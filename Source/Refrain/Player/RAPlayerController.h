// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "UI/RAHealthBarWidget.h"
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

	void InitHealthHUD(UAbilitySystemComponent* InASC);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<class UInputMappingContext> InputMappingContext;
	
	// HP UI
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<URAHealthBarWidget> HealthBarWidgetClass;
	
	UPROPERTY()
	TObjectPtr<URAHealthBarWidget> HealthBarWidget;
};
