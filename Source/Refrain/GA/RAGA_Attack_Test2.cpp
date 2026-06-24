// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_Attack_Test2.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "Refrain.h"
#include "RefrainGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AN_SendGameplayEvent.h"
#include "Animation/RACharacterAnimationData.h"
#include "Character/RACharacterBase.h"
#include "Component/AttackTargetingComponent.h"
#include "Timing/MagicalTimingSubsystem.h"

class UMotionWarpingComponent;

URAGA_Attack_Test2::URAGA_Attack_Test2()
{
	FGameplayTagContainer Tags(RefrainGameplayTags::Ability_Attack);
	SetAssetTags(Tags);;
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URAGA_Attack_Test2::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	
	// AN_SendGameplayEvent로부터 받을 태그로 델리게이트 등록
	UAbilityTask_WaitGameplayEvent* AttackHitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, RefrainGameplayTags::Event_Montage_AttackHit, nullptr, false, true);
	AttackHitTask->EventReceived.AddDynamic(this, &URAGA_Attack_Test2::OnAttackHit);
	AttackHitTask->ReadyForActivation();
	
	UAbilityTask_WaitGameplayEvent* MontagePlayRateTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, RefrainGameplayTags::Event_Montage_PlayRate, nullptr, false, false);
	MontagePlayRateTask->EventReceived.AddDynamic(this, &URAGA_Attack_Test2::OnMontagePlayRate);
	MontagePlayRateTask->ReadyForActivation();
	
	PlayAttackMontage();
}

void URAGA_Attack_Test2::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	SetJudgement();
	CurrentCombo++;
}

void URAGA_Attack_Test2::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	RA_LOG(LogRefrain, Log, TEXT("Start"));
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	ClearAttackMotionWarpTarget();
	TargetActor = nullptr;
}

void URAGA_Attack_Test2::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_Attack_Test2::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URAGA_Attack_Test2::OnAttackHit(FGameplayEventData Payload)
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
}

void URAGA_Attack_Test2::OnMontagePlayRate(FGameplayEventData Payload)
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

void URAGA_Attack_Test2::PlayAttackMontage()
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
			StartupPlayRate);
	MontageTask->OnCompleted.AddDynamic(this, &URAGA_Attack_Test2::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URAGA_Attack_Test2::OnMontageInterrupted);
	MontageTask->ReadyForActivation();
}

UAnimMontage* URAGA_Attack_Test2::GetNextAttackAnimMontage() const
{
	const ARACharacterBase* RACharacter = Cast<ARACharacterBase>(AvatarCharacter);
	const URACharacterAnimationData* AnimationData = RACharacter->GetAnimationData();
	check(AnimationData);
	
	if (AnimationData->AttackMontages.IsEmpty())
	{
		RA_LOG(LogRefrain, Warning, TEXT("AttackMontages Array Empty"));
		return AnimationData->AttackMontage_1;
	}
	
	const int MontageArrayNum = AnimationData->AttackMontages.Num();
	
	return AnimationData->AttackMontages[CurrentCombo % MontageArrayNum];
}

void URAGA_Attack_Test2::UpdateAttackMotionWarpTarget()
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

void URAGA_Attack_Test2::ClearAttackMotionWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComponent = AvatarCharacter->FindComponentByClass<UMotionWarpingComponent>();
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveAllWarpTargets();
	}
}

float URAGA_Attack_Test2::FindGameplayEventNotifyTime(const UAnimMontage* Montage, const FGameplayTag EventTag) const
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

void URAGA_Attack_Test2::SetTargetActor()
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

void URAGA_Attack_Test2::SetJudgement()
{
	UMagicalTimingSubsystem* MagicalTiming = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>();
	if (!MagicalTiming)
	{
		RA_LOG(LogRefrain, Error, TEXT("MagicalTimingSubsystem Not Found"));
		JudgementTag = RefrainGameplayTags::Judge_Miss;
		return;
	}
	
	if (!MagicalTiming->IsMusicPlaying())
	{
		RA_LOG(LogRefrain, Warning, TEXT("Music Not Playing"));
		JudgementTag = RefrainGameplayTags::Judge_Miss;
		return;
	}
	
	const float TimingDifference = MagicalTiming->JudgeTiming();
	
	if (!TargetActor)
	{
		JudgementTag = RefrainGameplayTags::Judge_Miss;
	}
	else if (TimingDifference < 0.05f)
	{
		JudgementTag = RefrainGameplayTags::Judge_Perfect;
	}
	else if (TimingDifference < 0.2f)
	{
		JudgementTag = RefrainGameplayTags::Judge_Good;
	}
	else
	{
		JudgementTag = RefrainGameplayTags::Judge_Bad;
	}
	
	RA_LOG(LogRefrain, Log, TEXT("JudgementTag: %s"), *JudgementTag.ToString());
}

float URAGA_Attack_Test2::GetDamageAmount() const
{
	return 10.0f;
}

void URAGA_Attack_Test2::CalculatePlayRates(const UAnimMontage* Montage)
{
	// 실제 박자 대비 애니메이션이 전환될 타이밍
	const float StartupToAnticipationInBeatProgress = 0.4f;
	const float AnticipationToStrikeInBeatProgress = 0.9f;
	const float StrikeToRecoveryInBeatProgress = 1.1f;
	
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
	TargetTime1 = TargetTime1 ? TargetTime1 : UE_KINDA_SMALL_NUMBER;
	StartupPlayRate = MontageTime1 / TargetTime1;
	
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
