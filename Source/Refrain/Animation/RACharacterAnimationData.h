// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RACharacterAnimationData.generated.h"

USTRUCT(BlueprintType)
struct FRAAttackData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector MotionWarpLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KnockbackDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageMultiplier = 1.f;
};

/**
 * 
 */
UCLASS()
class REFRAIN_API URACharacterAnimationData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	TArray<FRAAttackData> ComboAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	FRAAttackData CounterAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HitReact")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Death")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dodge")
	TObjectPtr<UAnimMontage> DodgeMontage;
};
