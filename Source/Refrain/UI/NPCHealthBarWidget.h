// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GA/Attribute/RAAttributeSet.h"
#include "NPCHealthBarWidget.generated.h"

class UAbilitySystemComponent;
class UProgressBar;

/**
 * 
 */
UCLASS()
class REFRAIN_API UNPCHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 체력바 표시 설정하는 함수,
	void SetOwnerASC(UAbilitySystemComponent* InASC);
	
protected:
	// ProgressBar
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Health;
	
	// 체력 정보를 가져올 ASC
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> OwnerASC;
	
	// 체력 값이 변경될 때 호출될 함수.
	virtual void OnHealthChanged(const FOnAttributeChangeData& Data);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	
	// 체력바 업데이트
	void UpdateHealthBar();
};
