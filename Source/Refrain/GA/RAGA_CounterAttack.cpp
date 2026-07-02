// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/RAGA_CounterAttack.h"

#include "Refrain.h"
#include "RefrainGameplayTags.h"
#include "Character/RACharacterBase.h"

URAGA_CounterAttack::URAGA_CounterAttack()
{
	FGameplayTagContainer Tags(RefrainGameplayTags::Ability_Attack);
	SetAssetTags(Tags);;
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URAGA_CounterAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	
	
}

void URAGA_CounterAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URAGA_CounterAttack::OnMontagePlayRate(FGameplayEventData Payload)
{
}
