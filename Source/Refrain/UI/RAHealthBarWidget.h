// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffect.h"
#include "RAHealthBarWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class REFRAIN_API URAHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitAbilitySystem(UAbilitySystemComponent* InASC);
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HealthText;
	
private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	void RefreshHealth();
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	
	
	
};
