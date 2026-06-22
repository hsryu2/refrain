// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_NPCAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/RACharacterNonPlayer.h"

URAGA_NPCAttack::URAGA_NPCAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URAGA_NPCAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 스킬 발동 시: NPC의 타겟팅 위젯을 켭니다.
	if (ARACharacterNonPlayer* NPC = Cast<ARACharacterNonPlayer>(ActorInfo->AvatarActor.Get()))
	{
		NPC->SetRhythmWidgetVisibility(true);
	}
	
	if (AttackMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("Attack"), AttackMontage); // 지정된 변수 사용

		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &URAGA_NPCAttack::OnMontageFinished);
			MontageTask->OnInterrupted.AddDynamic(this, &URAGA_NPCAttack::OnMontageFinished);
			MontageTask->ReadyForActivation();
		}
	}
	else
	{
		// 자산이 지정되지 않았을 때를 대비한 예외 처리
		UE_LOG(LogTemp, Warning, TEXT("AttackMontage is NOT set in %s"), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	
	
	// 애니메이션이 끝나면 꼭 EndAbility()를 호출해야 합니다.
}

void URAGA_NPCAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 스킬 종료 시: 위젯을 다시 끕니다.
	if (ARACharacterNonPlayer* NPC = Cast<ARACharacterNonPlayer>(ActorInfo->AvatarActor.Get()))
	{
		NPC->SetRhythmWidgetVisibility(false);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URAGA_NPCAttack::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
