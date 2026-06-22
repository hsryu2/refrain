// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/RACharacterPlayer.h"
#include "RAGA_Attack_Test1.generated.h"

/**
 * 
 */
UCLASS()
class REFRAIN_API URAGA_Attack_Test1 : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	URAGA_Attack_Test1();
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	

protected:
// 델리게이트로 실행되는 함수
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
	
	UFUNCTION()
	void OnAttackHit(FGameplayEventData Payload);
	
// 공격 실행
	void PlayAttackMontage();
	
// 모션 워핑 (몽타주 재생 전, 후 실행)
	void UpdateAttackMotionWarpTarget();
	void ClearAttackMotionWarpTarget();
	
// 재생 속도 조절
	// 몽타주 안에서 해당 몽타주 안의 UAnimNotify_SendGameplayEvent 노티파이가 위치한 시간을 반환하는 함수. 실패 시 -1.f 반환
	float FindGameplayEventNotifyTime(const UAnimMontage* Montage, const FGameplayTag EventTag = FGameplayTag::EmptyTag);
	
	// 몽타주 안의 노티파이까지 시간과 공격이 적중해야 할 시간을 비교해서 몽타주 Play Rate(선딜)을 결정
	float CalculateAttackPlayRate(float NotifyTime, float MinimumStartupDelay = 0.f);

// 변수
	UPROPERTY()
	TObjectPtr<ARACharacterPlayer> AvatarActor;
	
	// 콤보
	int CurrentCombo = 0;
	int MaxComboCount = 3;
	
	// 콤보 예약을 한 상태인지 확인.
	bool bComboInput;
	
	// 콤보 예약이 가능한 상태인지 확인.
	bool bCanAcceptComboInput;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	float DamageAmount = 10.0f;
	
	// 타겟팅 대상 캐싱
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
};

