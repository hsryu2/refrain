// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/RACharacterPlayer.h"
#include "RAGA_Attack.generated.h"

/**
 * 
 */
UCLASS()
class REFRAIN_API URAGA_Attack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	URAGA_Attack();
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	

protected:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();
	
	UFUNCTION()
	void OnComboStart(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboEnd(FGameplayEventData Payload);
	
	void PlayAttackMontage();
	
	UPROPERTY()
	TObjectPtr<ARACharacterPlayer> PlayerCharacter;
	
	// 콤보
	int CurrentCombo = 0;
	int MaxComboCount = 3;
	
	// 콤보 예약을 한 상태인지 확인.
	bool bComboInput;
	
	// 콤보 예약이 가능한 상태인지 확인.
	bool bCanAcceptComboInput;
	
	bool bChangingCombo;
	
	
};

