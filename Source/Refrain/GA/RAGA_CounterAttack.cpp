// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/RAGA_CounterAttack.h"

#include "AbilitySystemComponent.h"
#include "GameplayCueNotifyTypes.h"
#include "MotionWarpingComponent.h"
#include "Refrain.h"
#include "RefrainGameplayTags.h"
#include "RootMotionModifier.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/RACharacterAnimationData.h"
#include "Character/RACharacterBase.h"
#include "Character/RACharacterNonPlayer.h"
#include "Component/NPCCombatStateComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RAPlayerState.h"
#include "Timing/MagicalTimingSubsystem.h"
#include "Util/RAUtils.h"

URAGA_CounterAttack::URAGA_CounterAttack()
{
	FGameplayTagContainer Tags(RefrainGameplayTags::Ability_Attack_Counter);
	SetAssetTags(Tags);;

	ActivationOwnedTags.AddTag(RefrainGameplayTags::State_Attacking_Counter);
	
	ActivationBlockedTags.AddTag(RefrainGameplayTags::State_Dodging);
	
	CancelAbilitiesWithTag.AddTag(RefrainGameplayTags::Ability_Attack_Combo);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	bRetriggerInstancedAbility = true;
}

void URAGA_CounterAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 변수 초기화
	AvatarCharacter = Cast<ARACharacterBase>(ActorInfo->AvatarActor.Get());
	if (!AvatarCharacter)
	{
		RA_LOG(LogRefrain, Error, TEXT("AvatarCharacter Not Found"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Attacker = FindCounterableAttacker();
	bIsCounterSucceeded = CheckCounterSuccess();

	// AN_SendGameplayEvent로부터 받을 태그로 델리게이트 등록
	UAbilityTask_WaitGameplayEvent* AttackHitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, RefrainGameplayTags::Event_Montage_AttackHit, nullptr, false, false);
	AttackHitTask->EventReceived.AddDynamic(this, &URAGA_CounterAttack::OnAttackHit);
	AttackHitTask->ReadyForActivation();

	Attack();
}

void URAGA_CounterAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(RefrainGameplayTags::State_Invincible);

	ClearAttackMotionWarpTarget();

	AvatarCharacter = Attacker = nullptr;
	bIsCounterSucceeded = false;
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(RefrainGameplayTags::State_Attacking_Counter_Recovery);
	}
}

bool URAGA_CounterAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// 카운터 재실행을 위해 추가된 오버라이드 함수
	
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
	{
		return false;
	}
	
	// 클래스 타입 확인
	ARACharacterBase* CharacterBase = Cast<ARACharacterBase>(ActorInfo->AvatarActor.Get());
	if (!CharacterBase)
	{
		RA_LOG(LogRefrain, Error, TEXT("CharacterBase Cast Failed"));
	}
	
	UNPCCombatStateComponent* CombatManager = CharacterBase->FindComponentByClass<UNPCCombatStateComponent>();
	if (!CombatManager)
	{
		RA_LOG(LogRefrain, Error, TEXT("CombatManager Not Found"));
		return false;
	}

	// 공격 중인 적 확인
	ARACharacterNonPlayer* CurrentAttacker = CombatManager->GetCurrentAttacker();
	if (!IsValid(CurrentAttacker))
	{
		RA_LOG(LogRefrain, Log, TEXT("CurrentAttacker Not Found"));
		return false;
	}
	
	UAbilitySystemComponent* CurrentAttackerASC = CurrentAttacker->GetAbilitySystemComponent();
	if (!CurrentAttackerASC)
	{
		RA_LOG(LogRefrain, Error, TEXT("CurrentAttackerASC Not Found"));
		return false;
	}
	
	if (CurrentAttackerASC->HasMatchingGameplayTag(RefrainGameplayTags::State_Dead))
	{
		RA_LOG(LogRefrain, Warning, TEXT("CurrentAttacker is Dead"));
		return false;
	}
	
	// 카운터 재실행 확인
	if (IsActive() && Attacker == CurrentAttacker)
	{
		RA_LOG(LogRefrain, Log, TEXT("Attacker == CurrentAttacker"));
		return false;
	}
	
	// 카운터 가능 구간 확인
	if (!CurrentAttackerASC->HasMatchingGameplayTag(RefrainGameplayTags::State_Attacking_Counterable_InWindow))
	{
		RA_LOG(LogRefrain, Log, TEXT("Counter Failed"));
		return false;
	}
	
	RA_LOG(LogRefrain, Log, TEXT("return true;"));
	return true;
}

void URAGA_CounterAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_CounterAttack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URAGA_CounterAttack::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URAGA_CounterAttack::OnAttackHit(FGameplayEventData Payload)
{
	const float CounterAttackDamage = 45.f;
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Attacker);
	if (!ASC || !TargetASC)
	{
		RA_LOG(LogRefrain, Error, TEXT("ASC Not Found"));
		return;
	}
	
	// 대미지 전달(첫 타로 사망했을 경우 두 번째 타격에서는 대미지 적용 x)
	if (!TargetASC->HasMatchingGameplayTag(RefrainGameplayTags::State_Dead))
	{
		if (ASC && TargetASC)
		{
			FGameplayEffectSpecHandle DamageSpec = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
			DamageSpec.Data->SetSetByCallerMagnitude( RefrainGameplayTags::Data_Damage, CounterAttackDamage);
			RA_LOG(LogRefrain, Log, TEXT("Apply Damage: Target=%s Damage=%.1f"), *GetNameSafe(Attacker), CounterAttackDamage);
			ASC->ApplyGameplayEffectSpecToTarget(*DamageSpec.Data.Get(), TargetASC);
		}
		else
		{
			RA_LOG(LogRefrain, Log, TEXT("SourceASC or TargetASC Not Found"));
		}
	}
	
	// 넉백 적용
	FRAAttackData AttackData = AvatarCharacter->GetAnimationData()->CounterAttack;
	Payload.EventMagnitude = AttackData.KnockbackDistance;
	if (Payload.EventTag == RefrainGameplayTags::Event_Montage_AttackHit_FirstHit)
	{
		ASC->CurrentMontageSetPlayRate(PlayRateUntilSecondHit);
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Attacker, RefrainGameplayTags::State_HitReact, Payload);
	}
	else if (Payload.EventTag == RefrainGameplayTags::Event_Montage_AttackHit_SecondHit)
	{
		ASC->CurrentMontageSetPlayRate(PlayRateAfterSecondHit);
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Attacker, RefrainGameplayTags::State_HitReact, Payload);
		
		ASC->AddLooseGameplayTag(RefrainGameplayTags::State_Attacking_Counter_Recovery);
	}
	
	// 음악 재생 중이 아닐 경우 타격음 즉시 재생
	if (!GetWorld()->GetSubsystem<UMagicalTimingSubsystem>()->IsMusicPlaying())
	{
		const FRAHitSoundData* HitSoundData = nullptr;
		if (Payload.EventTag == RefrainGameplayTags::Event_Montage_AttackHit_FirstHit)
		{
			HitSoundData = GetHitSoundData(0);
		}
		else if (Payload.EventTag == RefrainGameplayTags::Event_Montage_AttackHit_SecondHit)
		{
			HitSoundData = GetHitSoundData(1);
		}
		
		if (HitSoundData && HitSoundData->HitSound)
		{
			UGameplayStatics::PlaySound2D(this, HitSoundData->HitSound);
		}
	}
}

void URAGA_CounterAttack::Attack()
{
	// 카운터 성공
	if (bIsCounterSucceeded && Attacker)
	{
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(RefrainGameplayTags::State_Invincible);
		SendJudgementToPlayerState(ERAHitJudgement::Perfect);
		PlayAttackMontage();
	}
	else
	{
		RA_LOG(LogRefrain, Log, TEXT("Counter Failed"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void URAGA_CounterAttack::PlayAttackMontage()
{
	// 카운터용 몽타주
	const ARACharacterBase* RACharacter = Cast<ARACharacterBase>(AvatarCharacter);
	check(RACharacter);
	const URACharacterAnimationData* AnimationData = RACharacter->GetAnimationData();
	check(AnimationData);
	const FRAAttackData CounterAttackData = AnimationData->CounterAttack;
	UAnimMontage* Montage = AnimationData->CounterAttack.Montage;
	check(Montage);

	// 사전 작업
	CalculatePlayRates(Montage);
	UpdateAttackMotionWarpTarget();
	QueueHitSound();

	// Montage AT 실행 및 델리게이트 등록
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("CounterAttack"),
			Montage,
			PlayRateUntilFirstHit,
			NAME_None,
			true,
			1.f,
			0.f);
	MontageTask->OnCompleted.AddDynamic(this, &URAGA_CounterAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URAGA_CounterAttack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &URAGA_CounterAttack::OnMontageCancelled);
	MontageTask->ReadyForActivation();
}

ARACharacterBase* URAGA_CounterAttack::FindCounterableAttacker()
{
	UNPCCombatStateComponent* NPCCombatStateManager = AvatarCharacter->FindComponentByClass<UNPCCombatStateComponent>();
	if (!NPCCombatStateManager)
	{
		RA_LOG(LogRefrain, Error, TEXT("NPCCombatStateManager Not Found"));
		return nullptr;
	}

	return NPCCombatStateManager->GetCurrentAttacker();
}

bool URAGA_CounterAttack::CheckCounterSuccess()
{
	if (!Attacker)
	{
		return false;
	}

	UAbilitySystemComponent* AttackerASC = Attacker->GetAbilitySystemComponent();
	if (!AttackerASC)
	{
		RA_LOG(LogRefrain, Error, TEXT("AttackerASC Not Found"));
		return false;
	}

	return AttackerASC->HasMatchingGameplayTag(RefrainGameplayTags::State_Attacking_Counterable_InWindow);
}

void URAGA_CounterAttack::CalculatePlayRates(const UAnimMontage* Montage)
{
	UMagicalTimingSubsystem* MagicalTiming = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>();
	if (!MagicalTiming || !MagicalTiming->IsMusicPlaying())
	{
		RA_LOG(LogRefrain, Warning, TEXT("Music Not Playing"));
		PlayRateUntilFirstHit = PlayRateUntilSecondHit = PlayRateAfterSecondHit = 1.f;
		return;
	}

	// 몽타주 내의 정보
	const float FirstHitTime = URAUtils::FindGameplayEventNotifyTime(
		Montage, RefrainGameplayTags::Event_Montage_AttackHit_FirstHit);
	const float SecondHitTime = URAUtils::FindGameplayEventNotifyTime(
		Montage, RefrainGameplayTags::Event_Montage_AttackHit_SecondHit);

	// 현재 재생 상태 정보
	const float SecondsPerBeat = MagicalTiming->GetSecondsPerBeat();
	FQuartzTransportTimeStamp MusicTimeStamp;
	MagicalTiming->GetMusicTimeStamp(MusicTimeStamp);
	const float BeatProgress = MusicTimeStamp.BeatFraction;
	const int NowBar = MusicTimeStamp.Bars;
	const int NowBeat = MusicTimeStamp.Beat;
	UMagicalMusicData* MusicData = MagicalTiming->GetMusicData();
	const int NumBeats = MusicData->NumBeats;

	// NPC 공격 정보
	UNPCCombatStateComponent* CombatManager = AvatarCharacter->FindComponentByClass<UNPCCombatStateComponent>();
	check(CombatManager);
	FAttackTiming NowAttackTiming = CombatManager->GetNowAttackTiming();
	const int AttackBar = NowAttackTiming.Bar;
	const int AttackBeat = NowAttackTiming.Beat;

	if (FirstHitTime <= 0.f || SecondHitTime <= 0.f)
	{
		RA_LOG(LogRefrain, Error, TEXT("FirstHitTime: %.2f SecondHitTime: %.2f BeatProgress: %.2f"), FirstHitTime,
		       SecondHitTime, BeatProgress);
		PlayRateUntilFirstHit = PlayRateUntilSecondHit = PlayRateAfterSecondHit = 1.f;
		return;
	}

	const int NowAbsoluteBeat = ((NowBar - 1) * NumBeats) + (NowBeat - 1);
	const int AttackAbsoluteBeat = ((AttackBar - 1) * NumBeats) + (AttackBeat - 1);
	const int BeatDifference = AttackAbsoluteBeat - NowAbsoluteBeat;

	// PlayRate 계산 - FirstHit가 0.5박, SecondHit가 1박에 맞춰지게
	const float DesiredFirstHitTime = (static_cast<float>(BeatDifference) + 0.5f - BeatProgress) * SecondsPerBeat;
	const float DesiredSecondHitTime = (static_cast<float>(BeatDifference) + 1.f - BeatProgress) * SecondsPerBeat;
	
	// 방어 코드
	if (DesiredFirstHitTime <= UE_KINDA_SMALL_NUMBER || DesiredSecondHitTime <= DesiredFirstHitTime)
	{
		RA_LOG(LogRefrain, Error, TEXT("DesiredFirstHitTime: %.2f DesiredSecondHitTime: %.2f, NowAbsoluteBeat: %d, AttackAbsoluteBeat: %d"), 
			DesiredFirstHitTime, DesiredSecondHitTime, NowAbsoluteBeat, AttackAbsoluteBeat);
		PlayRateUntilFirstHit = PlayRateUntilSecondHit = PlayRateAfterSecondHit = 1.f;
		return;
	}

	PlayRateUntilFirstHit = FirstHitTime / DesiredFirstHitTime;
	PlayRateUntilSecondHit = (SecondHitTime - FirstHitTime) / (DesiredSecondHitTime - DesiredFirstHitTime);

	PlayRateAfterSecondHit = 1.f;

	RA_LOG(LogRefrain, Log,
	       TEXT(
		       "BeatProgress: %.2f, PlayRateUntilFirstHit: %.2f, PlayRateUntilSecondHit: %.2f, PlayRateAfterSecondHit: %.2f"
	       ), BeatProgress, PlayRateUntilFirstHit, PlayRateUntilSecondHit, PlayRateAfterSecondHit);
}

void URAGA_CounterAttack::UpdateAttackMotionWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComponent = AvatarCharacter->FindComponentByClass<UMotionWarpingComponent>();
	if (!MotionWarpingComponent)
	{
		// 모션 워핑 컴포넌트가 없는 경우 그냥 바로 회전만
		RA_LOG(LogRefrain, Log, TEXT("MotionWarpingComponent Not Found"));
		FVector Direction =
			Attacker->GetActorLocation() - AvatarCharacter->GetActorLocation();
		Direction.Z = 0.0f;
		if (!Direction.IsNearlyZero())
		{
			FRotator TargetRotation = Direction.Rotation();
			AvatarCharacter->SetActorRotation(TargetRotation);
		}
	}
	else
	{
		// 적 정보		
		ACharacter* TargetCharacter = Cast<ACharacter>(Attacker);
		if (!TargetCharacter)
		{
			RA_LOG(LogRefrain, Error, TEXT("TargetCharacter Cast Failed"));
			return;
		}
		USkeletalMeshComponent* TargetMesh = TargetCharacter->GetMesh();
		if (!TargetMesh)
		{
			RA_LOG(LogRefrain, Error, TEXT("TargetMesh Not Found"));
			return;
		}

		// 애니메이션 정보에서 오프셋 가져오기
		const URACharacterAnimationData* AnimationData = AvatarCharacter->GetAnimationData();
		check(AnimationData);
		const FVector Offset = AnimationData->CounterAttack.MotionWarpLocationOffset;

		// 모션워핑에 필요한 정보 설정
		MotionWarpingComponent->AddOrUpdateWarpTargetFromComponent(
			FName(TEXT("Enemy")), TargetMesh, NAME_None, true,
			EWarpTargetLocationOffsetDirection::VectorFromTargetToOwner, Offset);
	}
}

void URAGA_CounterAttack::ClearAttackMotionWarpTarget()
{
	if (AvatarCharacter)
	{
		UMotionWarpingComponent* MotionWarpingComponent = AvatarCharacter->FindComponentByClass<
			UMotionWarpingComponent>();
		if (MotionWarpingComponent)
		{
			MotionWarpingComponent->RemoveAllWarpTargets();
		}
	}
}

const FRAHitSoundData* URAGA_CounterAttack::GetHitSoundData(int Index) const
{
	const URACharacterAnimationData* AnimationData = AvatarCharacter->GetAnimationData();
	check(AnimationData);
	
	if (AnimationData->CounterAttack.HitSoundData.Num() <= Index)
	{
		RA_LOG(LogRefrain, Error, TEXT("HitSoundData Array Index Out Of Range"));
		return nullptr;
	}
	
	const FRAAttackData& AttackData = AnimationData->CounterAttack;

	if (!AttackData.HitSoundData.IsValidIndex(Index))
	{
		RA_LOG(LogRefrain, Warning, TEXT("HitSoundData Array Empty, Index: %d"), Index);
		return nullptr;
	}

	return &AttackData.HitSoundData[Index];
}

void URAGA_CounterAttack::QueueHitSound()
{
	const URACharacterAnimationData* AnimationData = AvatarCharacter->GetAnimationData();
	check(AnimationData);

	const FRAHitSoundData* FirstHitSoundData = GetHitSoundData(0);
	const FRAHitSoundData* SecondHitSoundData = GetHitSoundData(1);
	check(FirstHitSoundData);
	check(SecondHitSoundData);
	check(FirstHitSoundData->HitSound);
	check(SecondHitSoundData->HitSound);
	
	UMagicalTimingSubsystem* MagicalTiming = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>();
	if (!MagicalTiming)
	{
		RA_LOG(LogRefrain, Error, TEXT("MagicalTimingSubsystem Not Found"));
		return;
	}
	
	if (!MagicalTiming->IsMusicPlaying())
	{
		RA_LOG(LogRefrain, Warning, TEXT("Music Not Playing"));
		return;
	}
	
	UNPCCombatStateComponent* CombatManager = AvatarCharacter->GetComponentByClass<UNPCCombatStateComponent>();
	if (!CombatManager)
	{
		RA_LOG(LogRefrain, Error, TEXT("CombatManager Not Found"));
		return;
	}
	
	FAttackTiming NowAttackTiming = CombatManager->GetNowAttackTiming();
	if (!NowAttackTiming.IsValid())
	{
		RA_LOG(LogRefrain, Error, TEXT("NowAttackTiming Is Not Valid"));
		return;
	}
	
	UMagicalMusicData* MusicData = MagicalTiming->GetMusicData();
	const int NumBeats = MusicData->NumBeats;
	
	const int AttackBar = NowAttackTiming.Bar;
	const int AttackBeat = NowAttackTiming.Beat;
	
	const int AttackAbsoluteBeat = ((AttackBar - 1) * NumBeats) + (AttackBeat - 1);

	if (FirstHitSoundData->Quantization != EQuartzCommandQuantization::EighthNote ||
		SecondHitSoundData->Quantization != EQuartzCommandQuantization::Beat)
	{
		RA_LOG(LogRefrain, Error,
		       TEXT("TransportRelative multiplier calculation requires First=EighthNote and Second=Beat"));
		return;
	}

	// Transport 시작점을 기준으로 FirstHit는 공격 목표 + 0.5박, SecondHit는 공격 목표 + 1박에 예약한다.
	const float FirstHitMultiplier = AttackAbsoluteBeat * 2.f + 2.f;
	const float SecondHitMultiplier = AttackAbsoluteBeat + 2.f;
	
	// 서브시스템에 타격음 재생 예약
	RA_LOG(LogRefrain, Log, TEXT("HitSound Queued"));
	MagicalTiming->PlaySFXQuantized(FirstHitSoundData->HitSound, FirstHitSoundData->Quantization,
	                                 FirstHitMultiplier, FirstHitSoundData->Offset,
	                                 EQuarztQuantizationReference::TransportRelative);
	MagicalTiming->PlaySFXQuantized(SecondHitSoundData->HitSound, SecondHitSoundData->Quantization,
	                                 SecondHitMultiplier, SecondHitSoundData->Offset,
	                                 EQuarztQuantizationReference::TransportRelative);
}

void URAGA_CounterAttack::SendJudgementToPlayerState(ERAHitJudgement Judgement)
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return;
	}
	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	if (!Pawn)
	{
		return;
	}
	ARAPlayerState* PlayerState = Pawn->GetPlayerState<ARAPlayerState>();
	if (!PlayerState)
	{
		return;
	}
	PlayerState->RegisterJudgement(Judgement);
}
