// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/RACharacterNonPlayer.h"
#include "Component/NPCCombatStateComponent.h"
#include "Player/RAPlayerState.h"
#include "RAGA_CounterAttack.generated.h"

struct FRAHitSoundData;
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
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();
	UFUNCTION()
	void OnMontageCancelled();
	
	UFUNCTION()
	void OnAttackHit(FGameplayEventData Payload);

protected:
	// 공격
	void Attack();
	
	// 공격 애니메이션 실행
	void PlayAttackMontage();

	// 카운터 가능한 공격을 하고 있는 대상 탐색
	ARACharacterBase* FindCounterableAttacker();
	
	// 카운터 가능한 시점인지 확인
	bool CheckCounterSuccess();
	
	// 애니메이션 재생 속도 계산 함수
	void CalculatePlayRates(const UAnimMontage* Montage);
	
	// 모션 워핑 (몽타주 재생 전, 후 실행)
	void UpdateAttackMotionWarpTarget();
	void ClearAttackMotionWarpTarget();
	
	const FRAHitSoundData* GetHitSoundData(int Index) const;
	
	// 사운드 예약 등록
	void QueueHitSound();

protected:
	// 실행한 캐릭터(플레이어 자신)
	UPROPERTY()
	TObjectPtr<ARACharacterBase> AvatarCharacter;
	
	// 카운터 공격한 캐릭터
	UPROPERTY()
	TObjectPtr<ARACharacterBase> Attacker;
	
	// 카운터 성공 여부
	UPROPERTY()
	bool bIsCounterSucceeded;
	
	// 재생 속도
	float PlayRateUntilFirstHit = 1.f;
	float PlayRateUntilSecondHit = 1.f;
	float PlayRateAfterSecondHit = 1.f;
	
protected:
	// PlayState에 점수 계산을 위해 판정 전달.
	void SendJudgementToPlayerState(ERAHitJudgement Judgement);
	
	// 블루프린트에서 설정할 변수
	// 대미지 GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
