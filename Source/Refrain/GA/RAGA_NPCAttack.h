// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RAGA_NPCAttack.generated.h"

/**
 * @brief NonPlayerCharacter가 공격 할 때를 다루는 Ability
 * @author sejong
 * @date 2026-06-22
 */
UCLASS()
class REFRAIN_API URAGA_NPCAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	URAGA_NPCAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION()
	void OnMontageFinished();
protected:
	/** NPC가 태어날 때 가질 기본 어빌리티 목록 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;
	
	/** 애님 몽타주를 가리킬 변수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AttackMontage;
};
