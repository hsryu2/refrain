// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_ComboAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "Refrain.h"
#include "RefrainGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AN_SendGameplayEvent.h"
#include "Animation/RACharacterAnimationData.h"
#include "Attribute/RAAttributeSet.h"
#include "Character/RACharacterBase.h"
#include "Component/AttackTargetingComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Timing/MagicalTimingSubsystem.h"

class UMotionWarpingComponent;

URAGA_ComboAttack::URAGA_ComboAttack()
{
	FGameplayTagContainer Tags(RefrainGameplayTags::Ability_Attack);
	SetAssetTags(Tags);
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URAGA_ComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	RA_LOG(LogRefrain, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 변수 초기화
	AvatarCharacter = Cast<ARACharacterBase>(ActorInfo->AvatarActor.Get());
	if (!AvatarCharacter)
	{
		RA_LOG(LogRefrain, Error, TEXT("AvatarCharacter Not Found"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	TargetingComponent = AvatarCharacter->FindComponentByClass<UAttackTargetingComponent>();
	CurrentCombo = 0;
	
	// 타겟이 있어야 판정이 인정되므로 먼저 타겟팅을 수행
	SetTargetActor();
	
	// 첫 타에 대한 타이밍 판정 수행
	CurrentJudgementTag = SetJudgement();
	
	// AN_SendGameplayEvent로부터 받을 태그로 델리게이트 등록
	UAbilityTask_WaitGameplayEvent* AttackHitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, RefrainGameplayTags::Event_Montage_AttackHit, nullptr, false, true);
	AttackHitTask->EventReceived.AddDynamic(this, &URAGA_ComboAttack::OnAttackHit);
	AttackHitTask->ReadyForActivation();
	
	UAbilityTask_WaitGameplayEvent* MontagePlayRateTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, RefrainGameplayTags::Event_Montage_PlayRate, nullptr, false, false);
	MontagePlayRateTask->EventReceived.AddDynamic(this, &URAGA_ComboAttack::OnMontagePlayRate);
	MontagePlayRateTask->ReadyForActivation();
	
	UAbilityTask_WaitGameplayEvent* NextComboStartTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, RefrainGameplayTags::Event_Montage_NextComboStart, nullptr, false, true);
	NextComboStartTask->EventReceived.AddDynamic(this, &URAGA_ComboAttack::OnNextComboStart);
	NextComboStartTask->ReadyForActivation();
	
	Attack();
}

void URAGA_ComboAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	if (!bHasQueuedAttackInput)
	{
		SetNextCombo();
	}
}

void URAGA_ComboAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	RA_LOG(LogRefrain, Log, TEXT("Start"));
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	ClearAttackMotionWarpTarget();
	TargetActor = nullptr;
}

void URAGA_ComboAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_ComboAttack::OnMontageInterrupted()
{
	if (bIsMontageInterruptedByCombo)
	{
		bIsMontageInterruptedByCombo = false;
		return;
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URAGA_ComboAttack::OnAttackHit(FGameplayEventData Payload)
{
	// 대미지 적용
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (SourceASC && TargetASC)
	{
		FGameplayEffectSpecHandle DamageSpec = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
		DamageSpec.Data->SetSetByCallerMagnitude( RefrainGameplayTags::Data_Damage, GetDamageAmount());
		RA_LOG(LogRefrain, Log, TEXT("Apply Damage: Target=%s Damage=%.1f"), *GetNameSafe(TargetActor), GetDamageAmount());
		SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpec.Data.Get(), TargetASC);
	}
	else
	{
		RA_LOG(LogRefrain, Log, TEXT("SourceASC or TargetASC Not Found"));
	}
	
	// 노래 재생 중이 아닐 때 타격음 재생
	UMagicalTimingSubsystem* MagicalTiming = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>();
	if (!MagicalTiming || !MagicalTiming->IsMusicPlaying())
	{
		if (HitSound)
		{
			UGameplayStatics::PlaySound2D(this, HitSound);
		}
	}
}

void URAGA_ComboAttack::OnMontagePlayRate(FGameplayEventData Payload)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		RA_LOG(LogRefrain, Error, TEXT("ASC Not Found"));
		return;
	}
	
	if (Payload.EventTag == RefrainGameplayTags::Event_Montage_PlayRate_StartupToAnticipation)
	{
		ASC->CurrentMontageSetPlayRate(AnticipationPlayRate);
	}
	else if (Payload.EventTag == RefrainGameplayTags::Event_Montage_PlayRate_AnticipationToStrike)
	{
		ASC->CurrentMontageSetPlayRate(StrikePlayRate);
	}
	else if (Payload.EventTag == RefrainGameplayTags::Event_Montage_PlayRate_StrikeToRecovery)
	{
		ASC->CurrentMontageSetPlayRate(RecoveryPlayRate);
	}
	else
	{
		RA_LOG(LogRefrain, Warning, TEXT("Unknown Montage PlayRate EventTag: %s"), *Payload.EventTag.ToString());
	}
}

void URAGA_ComboAttack::OnNextComboStart(FGameplayEventData Payload)
{
	if (bHasQueuedAttackInput)
	{
		bIsMontageInterruptedByCombo = true;
		Attack();
	}
}

void URAGA_ComboAttack::Attack()
{
	RA_LOG(LogRefrain, Log, TEXT("Current Combo: %d"), CurrentCombo);
	if (CurrentCombo > 0)
	{
		CurrentJudgementTag = QueuedJudgementTag;
	}
	
	PlayAttackMontage();
	bHasQueuedAttackInput = false;
}

void URAGA_ComboAttack::PlayAttackMontage()
{
	// 다음 공격 몽타주
	UAnimMontage* AttackMontage = GetNextAttackAnimMontage();
	if (!AttackMontage)
	{
		RA_LOG(LogRefrain, Warning, TEXT("AttackMontage Not Found"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	CalculatePlayRates(AttackMontage);
	
	// 타겟팅
	SetTargetActor();
	if (TargetActor)
	{
		UpdateAttackMotionWarpTarget();
	}

	if (TargetActor)
	{
		QueueHitSound();
	}
	
	// 공격 GE 실행
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Instigator = AvatarCharacter;
		CueParams.SourceObject = this;
		CueParams.Location = AvatarCharacter->GetActorLocation();
		CueParams.Normal = AvatarCharacter->GetActorForwardVector();
		
		ASC->ExecuteGameplayCue(RefrainGameplayTags::GameplayCue_Attack, CueParams);
	}
	
	// 현재 콤보에 따라서 Montage AT 실행 및 델리게이트 등록
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			*FString::Printf(TEXT("AttackMontage_%d"), CurrentCombo),
			AttackMontage,
			StartupPlayRate,
			NAME_None,
			true,
			1.f,
			MontageStartTime);
	MontageTask->OnCompleted.AddDynamic(this, &URAGA_ComboAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URAGA_ComboAttack::OnMontageInterrupted);
	MontageTask->ReadyForActivation();
}

UAnimMontage* URAGA_ComboAttack::GetNextAttackAnimMontage() const
{
	const ARACharacterBase* RACharacter = Cast<ARACharacterBase>(AvatarCharacter);
	const URACharacterAnimationData* AnimationData = RACharacter->GetAnimationData();
	check(AnimationData);
	
	if (AnimationData->ComboAttacks.IsEmpty())
	{
		RA_LOG(LogRefrain, Warning, TEXT("AttackMontages Array Empty"));
		return nullptr;
	}
	
	const int MontageArrayNum = AnimationData->ComboAttacks.Num();
	
	return AnimationData->ComboAttacks[CurrentCombo % MontageArrayNum].Montage;
}

void URAGA_ComboAttack::UpdateAttackMotionWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComponent = AvatarCharacter->FindComponentByClass<UMotionWarpingComponent>();
	if (!MotionWarpingComponent)
	{
		// 모션 워핑 컴포넌트가 없는 경우 그냥 바로 회전만
		RA_LOG(LogRefrain, Log, TEXT("MotionWarpingComponent Not Found"));
		FVector Direction = 
			TargetActor->GetActorLocation() - AvatarCharacter->GetActorLocation();
		Direction.Z = 0.0f;
		if (!Direction.IsNearlyZero())
		{
			FRotator TargetRotation = Direction.Rotation();
			AvatarCharacter->SetActorRotation(TargetRotation);
		}
	}
	else
	{
		ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
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
		// 모션워핑에 필요한 정보 설정 (현재 오프셋 설정 안 됨)
		MotionWarpingComponent->AddOrUpdateWarpTargetFromComponent(
			FName(TEXT("Enemy")), TargetMesh, NAME_None, true, 
			EWarpTargetLocationOffsetDirection::VectorFromTargetToOwner);
	}
}

void URAGA_ComboAttack::ClearAttackMotionWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComponent = AvatarCharacter->FindComponentByClass<UMotionWarpingComponent>();
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveAllWarpTargets();
	}
}

float URAGA_ComboAttack::FindGameplayEventNotifyTime(const UAnimMontage* Montage, const FGameplayTag EventTag) const
{
	if (!Montage)
	{
		return -1.f;
	}
	
	for (const FAnimNotifyEvent& NotifyEvent : Montage->Notifies)
	{
		const UAN_SendGameplayEvent* EventNotify = Cast<UAN_SendGameplayEvent>(NotifyEvent.Notify);
		if (EventNotify && EventNotify->EventTag == EventTag)
		{
			return NotifyEvent.GetTime();
		}
	}
	
	return -1.f;
}

void URAGA_ComboAttack::SetTargetActor()
{
	if (!TargetActor)
	{
		TargetActor = TargetingComponent->FindAttackTarget();
		return;
	}
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		RA_LOG(LogRefrain, Warning, TEXT("TargetASC Not Found From TargetActor"));
		return;
	}
	
	if (TargetASC->HasMatchingGameplayTag(RefrainGameplayTags::State_Dead))
	{
		TargetActor = TargetingComponent->FindAttackTarget();
	}
}

FGameplayTag URAGA_ComboAttack::SetJudgement()
{
	UMagicalTimingSubsystem* MagicalTiming = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>();
	
	FGameplayTag ResultTag;
	
	if (!MagicalTiming)
	{
		RA_LOG(LogRefrain, Error, TEXT("MagicalTimingSubsystem Not Found"));
		ResultTag = RefrainGameplayTags::Judge_Miss;
		return ResultTag;
	}
	
	if (!MagicalTiming->IsMusicPlaying())
	{
		RA_LOG(LogRefrain, Warning, TEXT("Music Not Playing"));
		ResultTag = RefrainGameplayTags::Judge_Miss;
		return ResultTag;
	}
	
	// 입력 타이밍 오차를 절대값으로 확인.
	const float TimingDifference = MagicalTiming->JudgeTiming();
	const float AbsTimingDifference = FMath::Abs(TimingDifference);
	
	if (!TargetActor)
	{
		ResultTag = RefrainGameplayTags::Judge_Miss;
	}
	else if (AbsTimingDifference < 0.05f)
	{
		ResultTag = RefrainGameplayTags::Judge_Perfect;
	}
	else if (AbsTimingDifference < 0.2f)
	{
		ResultTag = RefrainGameplayTags::Judge_Good;
	}
	else
	{
		ResultTag = RefrainGameplayTags::Judge_Bad;
	}
	
	RA_LOG(LogRefrain, Log, TEXT("[콤보 %d] 입력 타이밍 오차: %.3f초 -> 판정 결과: %s"), CurrentCombo, TimingDifference, *ResultTag.ToString());
	
	return ResultTag;
}

float URAGA_ComboAttack::GetDamageAmount() const
{
	float BaseDamage = 0.0f;
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		const URAAttributeSet*  AttributeSet = ASC->GetSet<URAAttributeSet>();
		if (AttributeSet)
		{
			BaseDamage = AttributeSet->GetAttackPower();
		}
	}
	
	float Multiplier = 1.0f;

	if (CurrentJudgementTag == RefrainGameplayTags::Judge_Perfect)
	{
		Multiplier = 1.5f;
	}
	else if (CurrentJudgementTag == RefrainGameplayTags::Judge_Good)
	{
		Multiplier = 1.2f;
	}
	else if (CurrentJudgementTag == RefrainGameplayTags::Judge_Bad)
	{
		Multiplier = 0.8f;
	}
	else if (CurrentJudgementTag == RefrainGameplayTags::Judge_Miss)
	{
		Multiplier = 0.5f;
	}
	
	return BaseDamage * Multiplier;
}

void URAGA_ComboAttack::CalculatePlayRates(const UAnimMontage* Montage)
{
	// 실제 박자 대비 애니메이션이 전환될 타이밍
	const float StartupToAnticipationInBeatProgress = 0.4f;
	const float AnticipationToStrikeInBeatProgress = 0.9f;
	const float StrikeToRecoveryInBeatProgress = 1.1f;
	
	const float MaxPlayRate = 3.f;
	
	MontageStartTime = 0.f;
	HitSoundBeatMultiplier = 1.f;
	
	UMagicalTimingSubsystem* MagicalTiming = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>();
	if (!MagicalTiming || !MagicalTiming->IsMusicPlaying())
	{
		RA_LOG(LogRefrain, Warning, TEXT("Music Not Playing"));
		StartupPlayRate = AnticipationPlayRate = StrikePlayRate = RecoveryPlayRate = 1.f;
		return;
	}
	
	// 몽타주 내의 정보
	const float PlayLength = Montage->GetPlayLength();
	
	// 몽타주 안에서 태그가 위치한 시간
	const float MontageTime1 = FindGameplayEventNotifyTime(Montage, RefrainGameplayTags::Event_Montage_PlayRate_StartupToAnticipation);
	const float MontageTime2 = FindGameplayEventNotifyTime(Montage, RefrainGameplayTags::Event_Montage_PlayRate_AnticipationToStrike);
	const float MontageTime3 = FindGameplayEventNotifyTime(Montage, RefrainGameplayTags::Event_Montage_PlayRate_StrikeToRecovery);
	if (MontageTime1 < 0.f || MontageTime2 < 0.f || MontageTime3 < 0.f)
	{
		RA_LOG(LogRefrain, Error, TEXT("Montage Notify Time Not Found: %f, %f, %f"), MontageTime1, MontageTime2, MontageTime3);
		StartupPlayRate = AnticipationPlayRate = StrikePlayRate = RecoveryPlayRate = 1.f;
		return;
	}
	
	// 현재 재생 상태 정보
	const float BPM = MagicalTiming->GetBPM();
	const float SecondsPerBeat = MagicalTiming->GetSecondsPerBeat();
	const float BeatProgress = MagicalTiming->GetBeatProgress();
	
	// PlayRate 계산 - Startup에서 박자에 맞게 맞춤
	float TargetTime1 = BeatProgress < StartupToAnticipationInBeatProgress ?
		(StartupToAnticipationInBeatProgress - BeatProgress) * SecondsPerBeat :
		(1.f + StartupToAnticipationInBeatProgress - BeatProgress) * SecondsPerBeat;
	TargetTime1 = FMath::Max(TargetTime1, UE_KINDA_SMALL_NUMBER);
	StartupPlayRate = MontageTime1 / TargetTime1;
	HitSoundBeatMultiplier = BeatProgress < StartupToAnticipationInBeatProgress ? 1.f : 2.f;
	
	// 예외처리 - 초반구간 재생속도가 너무 빠를 경우 앞부분 스킵
	if (StartupPlayRate > MaxPlayRate)
	{
		MontageStartTime = FMath::Clamp(MontageTime1 - TargetTime1 * MaxPlayRate, 0.f, MontageTime1);
		StartupPlayRate = MaxPlayRate;
	}
	
	// 이미 박자에 맞는 상태
	AnticipationPlayRate = (MontageTime2 - MontageTime1) / ((AnticipationToStrikeInBeatProgress - StartupToAnticipationInBeatProgress) * SecondsPerBeat);
	StrikePlayRate = (MontageTime3 - MontageTime2) / ((StrikeToRecoveryInBeatProgress - AnticipationToStrikeInBeatProgress) * SecondsPerBeat);
	
	// 몽타주 전체 재생 시간과 BPM 비교 - 2박에 걸쳐 재생될 수 있는 속도
	RecoveryPlayRate = PlayLength / SecondsPerBeat;
	
	// RA_LOG(LogRefrain, Log, TEXT("BPM: %.2f SecondsPerBeat: %.2f BeatProgress: %.2f, PlayLength: %.2f"), BPM, SecondsPerBeat, BeatProgress, PlayLength);
	// RA_LOG(LogRefrain, Log, TEXT("TargetTime1: %.2f"), TargetTime1);
	// RA_LOG(LogRefrain, Log, TEXT("MontageTime1: %.2f MontageTime2: %.2f MontageTime3: %.2f"), MontageTime1, MontageTime2, MontageTime3);
	RA_LOG(LogRefrain, Log, TEXT("StartupPlayRate: %.2f AnticipationPlayRate: %.2f StrikePlayRate: %.2f RecoveryPlayRate: %.2f"),
		StartupPlayRate, AnticipationPlayRate, StrikePlayRate, RecoveryPlayRate);
}

void URAGA_ComboAttack::SetNextCombo()
{
	CurrentCombo++;
	QueuedJudgementTag = SetJudgement();
	bHasQueuedAttackInput = true;
}

void URAGA_ComboAttack::QueueHitSound()
{
	if (!HitSound)
	{
		RA_LOG(LogRefrain, Warning, TEXT("HitSound Not Found"));
		return;
	}
	UMagicalTimingSubsystem* MagicalTiming = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>();
	if (!MagicalTiming)
	{
		RA_LOG(LogRefrain, Error, TEXT("MagicalTimingSubsystem Not Found"));
		return;
	}
	
	// 서브시스템에 타격음 재생 예약
	if (MagicalTiming->IsMusicPlaying())
	{
		RA_LOG(LogRefrain, Log, TEXT("HitSound Queued"));
		MagicalTiming->PlaySFXQuantized(HitSound, EQuartzCommandQuantization::Beat, HitSoundBeatMultiplier);
	}
}

