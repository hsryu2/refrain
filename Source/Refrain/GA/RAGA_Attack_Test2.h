// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RAGA_Attack_Test2.generated.h"

class UAttackTargetingComponent;
class ARACharacterBase;
class ARACharacterPlayer;

/**
 * 공격 애니메이션 재생 중 입력이 들어오면 즉시 결과 판정 후 다음 콤보 공격 실행 예약.
 * 
 */
UCLASS()
class REFRAIN_API URAGA_Attack_Test2 : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	URAGA_Attack_Test2();
	
protected:
// 재정의 함수
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
// 델리게이트로 실행되는 함수
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* AnimMontage, const bool bInterrupted);
	
	UFUNCTION()
	void OnAttackHit();

protected:	
	// 공격 애니메이션 실행
	void PlayAttackMontage();
	
	// 다음 공격 애니메이션 탐색 - 반환
	UAnimMontage* GetNextAttackAnimMontage() const;
	
	// 모션 워핑 (몽타주 재생 전, 후 실행)
	void UpdateAttackMotionWarpTarget();
	void ClearAttackMotionWarpTarget();
	
	// 몽타주 안에서 해당 몽타주 안의 UAnimNotify_SendGameplayEvent 노티파이가 위치한 시간을 반환하는 함수. 실패 시 -1.f 반환
	float FindGameplayEventNotifyTime(const UAnimMontage* Montage, const FGameplayTag EventTag = FGameplayTag::EmptyTag) const;
	
	// 몽타주 안의 노티파이까지 시간과 공격이 적중해야 할 시간을 비교해서 몽타주 Play Rate(선딜)을 결정
	float CalculateAttackPlayRate(float NotifyTime, float MinimumStartupDelay = 0.f) const;

	// TargetActor 상태 검사 후 null이거나 죽어있으면 새로운 타겟 검색 
	void SetTargetActor();
	
	// 타이밍 판정 및 저장
	void SetJudgement();

protected:
// 블루프린트에서 설정할 변수
	// 대미지 GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
protected:
	// 실행한 캐릭터
	UPROPERTY()
	TObjectPtr<ARACharacterBase> AvatarCharacter;
	
	// 타겟팅 컴포넌트 캐싱 (null 가능)
	UPROPERTY()
	TObjectPtr<UAttackTargetingComponent> TargetingComponent;
	
	// 타겟팅 대상 캐싱 (null 가능)
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
	
	// 콤보
	int CurrentCombo = 0;
	
	// 콤보 예약을 한 상태인지 확인.
	bool bGoNextCombo;
	
	// 입력 타이밍 판정 결과 저장
	FGameplayTag JudgementTag;
};
