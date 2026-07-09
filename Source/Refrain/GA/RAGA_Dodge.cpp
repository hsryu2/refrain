// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_Dodge.h"
#include "RefrainGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

URAGA_Dodge::URAGA_Dodge()
{
	FGameplayTagContainer Tags(RefrainGameplayTags::Ability_Dodge);
	SetAssetTags(Tags);
	
	ActivationOwnedTags.AddTag(RefrainGameplayTags::State_Dodging);
	ActivationOwnedTags.AddTag(RefrainGameplayTags::State_Invincible);
	
	CancelAbilitiesWithTag.AddTag(RefrainGameplayTags::Ability_Attack);
	BlockAbilitiesWithTag.AddTag(RefrainGameplayTags::Ability_Attack);
}

void URAGA_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	PlayerCharacter = Cast<ARACharacterPlayer>(ActorInfo->AvatarActor.Get());
	if (!PlayerCharacter)
	{
		EndAbility(Handle,ActorInfo,ActivationInfo, true, true);
		return;
	}
	PlayDodgeMontage();
}

void URAGA_Dodge::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void URAGA_Dodge::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void URAGA_Dodge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URAGA_Dodge::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_Dodge::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URAGA_Dodge::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URAGA_Dodge::PlayDodgeMontage()
{
	UAnimMontage* DodgeMontage = PlayerCharacter->GetDodgeMontage();
	if (!DodgeMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("Dodge"),
			DodgeMontage
		);
	// 몽타주가 완료/취소/중단 시 EndAbility를 호출하기 위해 델리게이트로 등록.
	MontageTask->OnCompleted.AddDynamic(this, &URAGA_Dodge::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URAGA_Dodge::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &URAGA_Dodge::OnMontageCancelled);
	
	MontageTask->ReadyForActivation();
}
