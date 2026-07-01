// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/RAGA_HitReact.h"

#include "RefrainGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/RACharacterAnimationData.h"
#include "Character/RACharacterBase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

URAGA_HitReact::URAGA_HitReact()
{
	// 피격 모션이 재생될 때 공격하는 모션 차단
	CancelAbilitiesWithTag.AddTag(RefrainGameplayTags::State_Attacking);
	
	ActivationOwnedTags.AddTag(RefrainGameplayTags::State_HitReact);
}

void URAGA_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
	{
		if (AAIController* AIController = Cast<AAIController>(AvatarPawn->GetController()))
		{
			if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
			{
				BlackboardComp->SetValueAsBool(FName("IsHit"), true);
			}
		}
	}
	
	ARACharacterBase* Character = Cast<ARACharacterBase>(GetAvatarActorFromActorInfo());
	UAnimMontage* HitMontage = Character->GetAnimationData()->HitReactMontage;
	
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("HitReact"), HitMontage);
	
	MontageTask->OnCompleted.AddDynamic(this, &URAGA_HitReact::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URAGA_HitReact::OnMontageCompleted);
	
	MontageTask->ReadyForActivation();
}

void URAGA_HitReact::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void URAGA_HitReact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
	{
		if (AAIController* AIController = Cast<AAIController>(AvatarPawn->GetController()))
		{
			if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
			{
				BlackboardComp->SetValueAsBool(FName("IsHit"), false);
			}
		}
	}
}

void URAGA_HitReact::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
