// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RAGA_CounterAttack.generated.h"

class ARACharacterBase;
/**
 * 카운터 공격(2타)을 한 박자에 반으로 나눠 실행
 * 카운터 대상에 전용 피격 애니메이션 재생
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
	// 재생 속도 조절 노티파이
	UFUNCTION()
	void OnMontagePlayRate(FGameplayEventData Payload);
	
// 게임 로직
protected:
	
protected:
	// 실행한 캐릭터(플레이어 자신)
	UPROPERTY()
	TObjectPtr<ARACharacterBase> AvatarCharacter;
	
	// 카운터 공격한 캐릭터
	UPROPERTY()
	TObjectPtr<ARACharacterBase> Attacker;
};
