// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "RAPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class REFRAIN_API ARAPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ARAPlayerState();
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	
protected:
	UPROPERTY(EditAnywhere, Category=GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;
	
	
	
};
