// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_NPCCounterableAttack.h"

#include "Refrain.h"
#include "RefrainGameplayTags.h"
#include "TimerManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "Animation/AN_SendGameplayEvent.h"
#include "AT/RAAT_RhythmTargetWidgetProgress.h"
#include "Character/RACharacterNonPlayer.h"
#include "Engine/World.h"
#include "Timing/MagicalTimingSubsystem.h"
#include "UI/RhythmTargetWidget.h"

URAGA_NPCCounterableAttack::URAGA_NPCCounterableAttack()
{
	FGameplayTagContainer Tags(RefrainGameplayTags::State_Attacking_Counterable);
	SetAssetTags(Tags);
	
	ActivationOwnedTags.AddTag(RefrainGameplayTags::State_Attacking_Counterable);
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URAGA_NPCCounterableAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 변수 저장
	NPC = Cast<ARACharacterNonPlayer>(ActorInfo->AvatarActor.Get());
	if (!NPC)
	{
		RA_LOG(LogRefrain, Error, TEXT("NPC Not Found"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 공격
	Attack();
}

void URAGA_NPCCounterableAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URAGA_NPCCounterableAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_NPCCounterableAttack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URAGA_NPCCounterableAttack::Attack()
{
	if (!IsValid(AttackMontage))
	{
		RA_LOG(LogRefrain, Error, TEXT("AttackMontage Is Not Valid"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	float TargetBeatTime = CalculatePlayRate(AttackMontage);
	
	// WaitTime이 있을 경우 타이머 설정해서 몽타주 재생
	if (MontageWaitTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(MontageWaitTimerHandle, this, &URAGA_NPCCounterableAttack::PlayAttackMontage, MontageWaitTime, false);
	}
	else
	{
		PlayAttackMontage();
	}
	
	// 위젯 설정
	RhythmTargetWidget = NPC->GetRhythmTargetWidget();
	NPC->SetRhythmWidgetVisibility(true);
	if (!RhythmTargetWidget)
	{
		RA_LOG(LogRefrain, Error, TEXT("RhythmTargetWidget Not Found"));
	}
	
	// 위젯 업데이트하는 태스크
	URAAT_RhythmTargetWidgetProgress* RhythmTask =
		URAAT_RhythmTargetWidgetProgress::PlayRhythmTargetProgress(
			this,
			RhythmTargetWidget,
			TargetBeatTime
		);
	RhythmTask->ReadyForActivation();
	
}

void URAGA_NPCCounterableAttack::PlayAttackMontage()
{
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("NPCCounterableAttack"),
			AttackMontage,
			MontagePlayRate,
			NAME_None,
			true,
			1.f,
			MontageStartTime);
	MontageTask->OnCompleted.AddDynamic(this, &URAGA_NPCCounterableAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URAGA_NPCCounterableAttack::OnMontageInterrupted);
	MontageTask->ReadyForActivation();
}

float URAGA_NPCCounterableAttack::CalculatePlayRate(const UAnimMontage* Montage)
{
	const FGameplayTag TargetTag = RefrainGameplayTags::Event_Montage_AttackHit;
	const float TargetProgressOnTargetTagNotify = 1.1f;
	const float TargetBeatMultiplier = 2.f;
	const float MinPlayRate = 0.5f;
	const float MaxPlayRate = 2.f;
	
	// 기본값
	MontagePlayRate = 1.f;
	MontageStartTime = 0.f;
	MontageWaitTime = 0.f;
	
	const float NotifyTime = FindGameplayEventNotifyTime(Montage, TargetTag);
	if (NotifyTime <= 0.f)
	{
		RA_LOG(LogRefrain, Warning, TEXT("Montage Notify Time Not Found: %f"), NotifyTime);
		return -1.f;
	}
	
	UMagicalTimingSubsystem* MagicalTimingSubsystem = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>();
	if (!MagicalTimingSubsystem)
	{
		RA_LOG(LogRefrain, Warning, TEXT("MagicalTimingSubsystem Not Found"));
		
		return -1.f;
	}
	if (!MagicalTimingSubsystem->IsMusicPlaying())
	{
		RA_LOG(LogRefrain, Warning, TEXT("Music Not Playing"));
		return -1.f;
	}
	
	// 목표 박자 시간
	float TargetBeatTime = MagicalTimingSubsystem->GetTimeUntilNextHit(0.f, EQuartzCommandQuantization::Beat, TargetBeatMultiplier);
	
	// 계산식...
	MontagePlayRate = (TargetBeatTime * TargetProgressOnTargetTagNotify) / NotifyTime;

	if (MontagePlayRate > MaxPlayRate)
	{
		MontageStartTime = (MontagePlayRate - MaxPlayRate) / NotifyTime;

		MontagePlayRate = MaxPlayRate;
	}
	else if (MontagePlayRate < MinPlayRate)
	{
		MontageWaitTime = (MinPlayRate - MontagePlayRate) / NotifyTime;
		
		MontagePlayRate = MinPlayRate;
	}

	return TargetBeatTime;
}

float URAGA_NPCCounterableAttack::FindGameplayEventNotifyTime(const UAnimMontage* Montage, const FGameplayTag EventTag) const
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
