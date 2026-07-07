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
	
	/*// HitReact 중복 발동 방지
	ActivationBlockedTags.AddTag(RefrainGameplayTags::State_HitReact);*/
}

void URAGA_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (TriggerEventData)
	{
		float KnockbackDis = TriggerEventData->EventMagnitude;
		
		const AActor* InstigatorActor = TriggerEventData->Instigator;
		ACharacter* HitCharacter = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
		
		if (InstigatorActor && HitCharacter && KnockbackDis > 0.0f)
		{
			FVector LookAtDir = InstigatorActor->GetActorLocation() - HitCharacter->GetActorLocation();
			LookAtDir.Z = 0.0f;
			FRotator LookAtRotator = LookAtDir.Rotation();
			HitCharacter->SetActorRotation(LookAtRotator);
			
			FVector KnockbackDir = HitCharacter->GetActorLocation() - InstigatorActor->GetActorLocation();
			KnockbackDir.Z = 0.0f;
			KnockbackDir.Normalize();
			
			FVector KnockbackVelocity = KnockbackDir * KnockbackDis;
			HitCharacter->LaunchCharacter(KnockbackVelocity, true, true);
			
		}
	}
	
	// AIController를 통해 블랙보드에서 피격 상태로 변경
	if (APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
	{
		if (AAIController* AIController = Cast<AAIController>(AvatarPawn->GetController()))
		{
			if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
			{
				BlackboardComp->SetValueAsBool(FName("IsHit"), true);
				
				const bool bCurrentIsHit = BlackboardComp->GetValueAsBool(FName("IsHit"));
				UE_LOG(LogTemp, Warning, TEXT("Blackboard IsHit after set: %s / Pawn: %s / Controller: %s"),
				bCurrentIsHit ? TEXT("true") : TEXT("false"),
				*GetNameSafe(AvatarPawn),
				*GetNameSafe(AIController));
			}
		}
	}
	
	// 몽타주 등록
	ARACharacterBase* Character = Cast<ARACharacterBase>(GetAvatarActorFromActorInfo());
	UAnimMontage* HitMontage = Character->GetAnimationData()->HitReactMontage;
	
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("HitReact"), HitMontage);
	
	MontageTask->OnCompleted.AddDynamic(this, &URAGA_HitReact::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URAGA_HitReact::OnMontageInterrupted);
	
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
	// 몽타주 실행이 끝나면 피격 상태를 false로 변경
	if (APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
	{
		if (AAIController* AIController = Cast<AAIController>(AvatarPawn->GetController()))
		{
			if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
			{
				if (!bWasCancelled)
				{
					BlackboardComp->SetValueAsBool(FName("IsHit"), false);
				}

				const bool bCurrentIsHit = BlackboardComp->GetValueAsBool(FName("IsHit"));
				UE_LOG(LogTemp, Warning, TEXT("Blackboard IsHit after set: %s / Pawn: %s / Controller: %s"),
				bCurrentIsHit ? TEXT("true") : TEXT("false"),
				*GetNameSafe(AvatarPawn),
				*GetNameSafe(AIController));
			}
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URAGA_HitReact::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_HitReact::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
