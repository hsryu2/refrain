// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RAHealthBarWidget.h"
#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GA/Attribute/RAAttributeSet.h"
void URAHealthBarWidget::InitAbilitySystem(UAbilitySystemComponent* InASC)
{
	ASC = InASC;
	if (!ASC)
	{
		return;
	}
	// 델리게이트로 체력바에 현재 체력과 최대체력 등록
	ASC->GetGameplayAttributeValueChangeDelegate(URAAttributeSet::GetHealthAttribute())
		.AddUObject(this, &URAHealthBarWidget::OnHealthChanged);
	
	ASC->GetGameplayAttributeValueChangeDelegate(URAAttributeSet::GetMaxHealthAttribute())
	.AddUObject(this, &URAHealthBarWidget::OnMaxHealthChanged);
	
	RefreshHealth();
}

void URAHealthBarWidget::RefreshHealth()
{
	if (!ASC)
	{
		return;
	}
	
	const float Health = ASC->GetNumericAttribute(URAAttributeSet::GetHealthAttribute());
	const float MaxHealth = ASC->GetNumericAttribute(URAAttributeSet::GetMaxHealthAttribute());
	const float Percent = MaxHealth > 0.0f ? Health / MaxHealth : 0.0f;
	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}
	
	if (HealthText)
	{
		HealthText->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)	
		));
	}
}

void URAHealthBarWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	RefreshHealth();
}

void URAHealthBarWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	RefreshHealth();
}
