// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RACharacterAnimationData.generated.h"

/**
 * 
 */
UCLASS()
class REFRAIN_API URACharacterAnimationData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	TObjectPtr<UAnimMontage> AttackMontage_1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	TObjectPtr<UAnimMontage> AttackMontage_2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	TObjectPtr<UAnimMontage> AttackMontage_3;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dodge")
	TObjectPtr<UAnimMontage> DodgeMontage;
};
