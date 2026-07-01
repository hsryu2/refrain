// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RAGA_CounterAttack.generated.h"

/**
 * 
 */
UCLASS()
class REFRAIN_API URAGA_CounterAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	URAGA_CounterAttack();

// 재정의 함수
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
// 델리게이트로 실행되는 함수
protected:
	
protected:
	
};
