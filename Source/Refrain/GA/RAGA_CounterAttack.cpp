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
#include "Timing/MagicalTimingSubsystem.h"
#include "Util/RAUtils.h"

URAGA_CounterAttack::URAGA_CounterAttack()
{
	FGameplayTagContainer Tags(RefrainGameplayTags::Ability_Attack_Counter);
	SetAssetTags(Tags);;
	
	ActivationOwnedTags.AddTag(RefrainGameplayTags::State_Attacking);
	ActivationBlockedTags.AddTag(RefrainGameplayTags::State_Dodging);
	ActivationBlockedTags.AddTag(RefrainGameplayTags::State_Attacking);
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URAGA_CounterAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	RA_LOG(LogRefrain, Log, TEXT("Activate Ability"));
	
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
	
	AvatarCharacter = Attacker = nullptr;
	bIsCounterSucceeded = false;
}

void URAGA_CounterAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_CounterAttack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URAGA_CounterAttack::OnAttackHit(FGameplayEventData Payload)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		RA_LOG(LogRefrain, Error, TEXT("ASC Not Found"));
		return;
	}
	
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
	}
}

void URAGA_CounterAttack::Attack()
{
	// 카운터 성공
	if (bIsCounterSucceeded && Attacker)
	{
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
	const float FirstHitTime = URAUtils::FindGameplayEventNotifyTime(Montage, RefrainGameplayTags::Event_Montage_AttackHit_FirstHit);
	const float SecondHitTime = URAUtils::FindGameplayEventNotifyTime(Montage, RefrainGameplayTags::Event_Montage_AttackHit_SecondHit);
	
	// 현재 재생 상태 정보
	const float SecondsPerBeat = MagicalTiming->GetSecondsPerBeat();
	const float BeatProgress = MagicalTiming->GetBeatProgress();
	FQuartzTransportTimeStamp MusicTimeStamp;
	MagicalTiming->GetMusicTimeStamp(MusicTimeStamp);
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
		RA_LOG(LogRefrain, Error, TEXT("FirstHitTime: %.2f SecondHitTime: %.2f BeatProgress: %.2f"), FirstHitTime, SecondHitTime, BeatProgress);
		PlayRateUntilFirstHit = PlayRateUntilSecondHit = PlayRateAfterSecondHit = 1.f;
		return;
	}
	
	const int NowAbsoluteBeat = ((NowBar - 1) * NumBeats) + (NowBeat - 1);
	const int AttackAbsoluteBeat = ((AttackBar - 1) * NumBeats) + (AttackBeat - 1);
	const int BeatDifference = AttackAbsoluteBeat - NowAbsoluteBeat;
	
	// PlayRate 계산 - FirstHit가 0.5박, SecondHit가 1박에 맞춰지게
	const float DesiredFirstHitTime = (static_cast<float>(BeatDifference) + 0.5f - BeatProgress) * SecondsPerBeat;
	const float DesiredSecondHitTime = (static_cast<float>(BeatDifference) + 1.f - BeatProgress) * SecondsPerBeat;
	
	PlayRateUntilFirstHit = FirstHitTime / DesiredFirstHitTime;
	PlayRateUntilSecondHit = (SecondHitTime - FirstHitTime) / (DesiredSecondHitTime - DesiredFirstHitTime);
	
	PlayRateAfterSecondHit = 1.f;
	
	RA_LOG(LogRefrain, Log, TEXT("BeatProgress: %.2f, PlayRateUntilFirstHit: %.2f, PlayRateUntilSecondHit: %.2f, PlayRateAfterSecondHit: %.2f"), BeatProgress, PlayRateUntilFirstHit, PlayRateUntilSecondHit, PlayRateAfterSecondHit);
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
	UMotionWarpingComponent* MotionWarpingComponent = AvatarCharacter->FindComponentByClass<UMotionWarpingComponent>();
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveAllWarpTargets();
	}
}

void URAGA_CounterAttack::QueueHitSound()
{
	// TODO
}
