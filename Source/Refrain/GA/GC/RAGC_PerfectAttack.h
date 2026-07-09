// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "NiagaraSystem.h"
#include "RAGC_PerfectAttack.generated.h"

/**
 * Perfect 판정 공격 시 TargetActor 위치 기준으로 Niagara 이펙트를 스폰하는 GameplayCue.
 */
UCLASS()
class REFRAIN_API URAGC_PerfectAttack : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	URAGC_PerfectAttack();

protected:
	virtual bool OnExecute_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters
	) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect)
	TObjectPtr<UNiagaraSystem> PerfectAttackEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect)
	FVector TargetLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect)
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect)
	FVector SpawnScale = FVector(4.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect)
	bool bUseTargetRotation = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect)
	bool bOffsetInTargetLocalSpace = true;
};
