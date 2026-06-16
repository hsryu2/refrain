// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_Attack.h"
#include "AbilitySystemComponent.h"
#include "../RefrainGameplayTags.h"

URAGA_Attack::URAGA_Attack()
{
	AbilityTags.AddTag(RefrainGameplayTags::Ability_Attack);
}

void URAGA_Attack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ARACharacterPlayer* PlayerCharacter = Cast<ARACharacterPlayer>(ActorInfo->AvatarActor.Get());
	if (!PlayerCharacter)
	{
		EndAbility(Handle,ActorInfo,ActivationInfo, true, true);
		return;
	}
	UAnimMontage* AttackMontage = PlayerCharacter->GetAttackMontage();
	if (!AttackMontage)
	{
		EndAbility(Handle,ActorInfo,ActivationInfo, true, true);
		return;
	}
	
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Instigator = PlayerCharacter;
		CueParams.SourceObject = this;
		CueParams.Location = PlayerCharacter->GetActorLocation();
		CueParams.Normal = PlayerCharacter->GetActorForwardVector();
		
		ASC->ExecuteGameplayCue(RefrainGameplayTags::GameplayCue_Attack, CueParams);
	}
	
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("AttackMontage"),
			AttackMontage
		);
	
	// 몽타주가 완료/취소/중단 시 EndAbility를 호출하기 위해 델리게이트로 등록.
	MontageTask->OnCompleted.AddDynamic(this, &URAGA_Attack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URAGA_Attack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &URAGA_Attack::OnMontageCancelled);
	
	MontageTask->ReadyForActivation();
	

}

void URAGA_Attack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void URAGA_Attack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void URAGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URAGA_Attack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_Attack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URAGA_Attack::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
