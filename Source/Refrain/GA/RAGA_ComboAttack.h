// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RAGA_ComboAttack.generated.h"

class UAttackTargetingComponent;
class ARACharacterBase;
class ARACharacterPlayer;

/**
 * 몽타주 속도 설정 후 BPM에 맞춰 타격까지 재생하는 함수. 콤보 실행.
 * 몽타주 추가 시 애니메이션데이터 배열에 추가, 몽타주에 이벤트 태그 설정
 * 콤보 입력은 애니메이션 재생 시작시부터 NextComboStart 전까지 최초 1회만 받음
 * TODO: 판정 타이밍 저장 기능 완성 안 됨(대미지 관련 기능 포함)
 */
UCLASS()
class REFRAIN_API URAGA_ComboAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	URAGA_ComboAttack();
	
protected:
// 재정의 함수
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
// 델리게이트로 실행되는 함수
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();
	
	// 대미지 전달 시점
	UFUNCTION()
	void OnAttackHit(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnMontagePlayRate(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnNextComboStart(FGameplayEventData Payload);

protected:
	// 공격
	void Attack();
	
	// 공격 애니메이션 실행
	void PlayAttackMontage();
	
	// 다음 공격 애니메이션 탐색 - 반환
	UAnimMontage* GetNextAttackAnimMontage() const;
	
	// 모션 워핑 (몽타주 재생 전, 후 실행)
	void UpdateAttackMotionWarpTarget();
	void ClearAttackMotionWarpTarget();
	
	// 몽타주 안에서 해당 몽타주 안의 UAN_SendGameplayEvent 노티파이가 위치한 시간을 반환하는 함수. 실패 시 -1.f 반환
	float FindGameplayEventNotifyTime(const UAnimMontage* Montage, const FGameplayTag EventTag = FGameplayTag::EmptyTag) const;

	// TargetActor 상태 검사 후 null이거나 죽어있으면 새로운 타겟 검색 
	void SetTargetActor();
	
	// 타이밍 판정 및 저장
	FGameplayTag SetJudgement();
	
	// 대미지 계산
	float GetDamageAmount() const;
	
	// 애니메이션 재생 속도 계산 함수
	void CalculatePlayRates(const UAnimMontage* Montage);
	
	// 다음 콤보 예약
	void SetNextCombo();
	
	// 타격 효과음 재생
	void QueueHitSound();

protected:
// 블루프린트에서 설정할 변수
	// 대미지 GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	// 타격음
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	TObjectPtr<USoundBase> HitSound;
	
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
	bool bHasQueuedAttackInput = false;
	
	// 콤보로 인해 기존 몽타주가 중단될 때 이를 확인하는 변수
	bool bIsMontageInterruptedByCombo = false;
	
	// 입력 타이밍 판정 결과 저장
	// 입력 타이밍 판정 결과를 분리해서 저장
	// 다음 입력의 것을 받아두는 것.
	FGameplayTag CurrentJudgementTag;
	FGameplayTag QueuedJudgementTag;
	
	// 애니메이션 구간별 재생 속도
	float StartupPlayRate;
	float AnticipationPlayRate;
	float StrikePlayRate;
	float RecoveryPlayRate;
	float MontageStartTime;
	float HitSoundBeatMultiplier = 1.f;
};
