// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RAPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class REFRAIN_API ARAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ARAPlayerController();
	
	FORCEINLINE TObjectPtr<class UInputMappingContext> GetInputMappingContext() { return InputMappingContext; }

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<class UInputMappingContext> InputMappingContext;
};
