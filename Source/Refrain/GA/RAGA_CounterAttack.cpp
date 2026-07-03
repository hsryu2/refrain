// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/RAGA_CounterAttack.h"

#include "AbilitySystemComponent.h"
#include "Refrain.h"
#include "RefrainGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/RACharacterAnimationData.h"
#include "Character/RACharacterBase.h"
#include "Character/RACharacterNonPlayer.h"
#include "Component/NPCCombatStateComponent.h"
#include "Engine/World.h"
#include "Timing/MagicalTimingSubsystem.h"
#include "Util/RAUtils.h"

URAGA_CounterAttack::URAGA_CounterAttack()
{
	FGameplayTagContainer Tags(RefrainGameplayTags::Ability_Attack_Counter);
	SetAssetTags(Tags);;
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URAGA_CounterAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	
	if (Payload.EventTag == RefrainGameplayTags::Event_Montage_AttackHit_FirstHit)
	{
		ASC->CurrentMontageSetPlayRate(PlayRateUntilSecondHit);
		// TODO: 공격 이벤트 전달
	}
	else if (Payload.EventTag == RefrainGameplayTags::Event_Montage_AttackHit_SecondHit)
	{
		ASC->CurrentMontageSetPlayRate(PlayRateAfterSecondHit);
		// TODO: 공격 이벤트 전달
	}
}

void URAGA_CounterAttack::Attack()
{
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
	UAnimMontage* Montage = AnimationData->CounterAttack;
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
	
	return NPCCombatStateManager->GetCurrentMainAttacker();
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
	
	if (FirstHitTime <= 0.f || SecondHitTime <= 0.f || BeatProgress >= 0.5f)
	{
		RA_LOG(LogRefrain, Error, TEXT("FirstHitTime: %.2f SecondHitTime: %.2f BeatProgress: %.2f"), FirstHitTime, SecondHitTime, BeatProgress);
		PlayRateUntilFirstHit = PlayRateUntilSecondHit = PlayRateAfterSecondHit = 1.f;
		return;
	}
	
	// PlayRate 계산 - FirstHit가 0.5박, SecondHit가 1박에 맞춰지게
	const float DesiredFirstHitTime = (0.5f - BeatProgress) * SecondsPerBeat;
	const float DesiredSecondHitTime = (1.f - BeatProgress) * SecondsPerBeat;
	
	PlayRateUntilFirstHit = FirstHitTime / DesiredFirstHitTime;
	PlayRateUntilSecondHit = (SecondHitTime - FirstHitTime) / (DesiredSecondHitTime - DesiredFirstHitTime);
	
	PlayRateAfterSecondHit = 1.f;
	
	RA_LOG(LogRefrain, Log, TEXT("PlayRateUntilFirstHit: %.2f, PlayRateUntilSecondHit: %.2f, PlayRateAfterSecondHit: %.2f"), PlayRateUntilFirstHit, PlayRateUntilSecondHit, PlayRateAfterSecondHit);
}

void URAGA_CounterAttack::UpdateAttackMotionWarpTarget()
{
}

void URAGA_CounterAttack::ClearAttackMotionWarpTarget()
{
}

void URAGA_CounterAttack::QueueHitSound()
{
}
  