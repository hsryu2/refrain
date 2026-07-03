// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Components/WidgetComponent.h"
#include "RAGA_NPCCounterableAttack.generated.h"

class URhythmTargetWidget;
class ARACharacterNonPlayer;
/**
 * 
 */
UCLASS()
class REFRAIN_API URAGA_NPCCounterableAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URAGA_NPCCounterableAttack();

// 재정의 함수
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
// 델리게이트로 실행되는 함수
protected:
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();

// 내부 로직
protected:
	void Attack();
	
	// 공격 애니메이션 재생
	void PlayAttackMontage();
	
	// 재생 시간 계산. 타격 시점 박자까지 남은 시간 반환.
	float CalculatePlayRate(const UAnimMontage* Montage);
	
	// CombatStateComponent에 공격할 타이밍 저장
	void SetAttackTiming();
	void ClearAttackTiming();
	
// 블루프린트에서 지정할 변수
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
protected:
	UPROPERTY()
	TObjectPtr<UWidgetComponent> RhythmTargetWidget;
	
	UPROPERTY()
	TObjectPtr<ARACharacterNonPlayer> NPC;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	float DamageAmount = 10.0f;
	
	FTimerHandle MontageWaitTimerHandle;
	float MontagePlayRate = 1.f;
	float MontageStartTime = 0.f;
	float MontageWaitTime = 0.f;
	
	int AttackTimeStampBar = 0;
	int AttackTimeStampBeat = 0;
};
