// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPCHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "GA/Attribute/RAAttributeSet.h"

void UNPCHealthBarWidget::SetOwnerASC(UAbilitySystemComponent* InASC)
{
	if (OwnerASC.IsValid())
	{
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAAttributeSet::GetHealthAttribute()).RemoveAll(this);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAAttributeSet::GetMaxHealthAttribute()).RemoveAll(this);
	}
	
	OwnerASC = InASC;
	
	if (OwnerASC.IsValid())
	{
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAAttributeSet::GetHealthAttribute()).AddUObject(this, &UNPCHealthBarWidget::OnHealthChanged);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UNPCHealthBarWidget::OnMaxHealthChanged);
		UpdateHealthBar();
	}
}

void UNPCHealthBarWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	UpdateHealthBar();
}

void UNPCHealthBarWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UpdateHealthBar();
}

void UNPCHealthBarWidget::UpdateHealthBar()
{
	if (OwnerASC.IsValid() && PB_Health)
	{
		const URAAttributeSet* AttributeSet = OwnerASC->GetSet<URAAttributeSet>();
		if (AttributeSet)
		{
			float CurrentHealth = AttributeSet->GetHealth();
			float MaxHealth = AttributeSet->GetMaxHealth();
			
			if (MaxHealth > 0.0f)
			{
				PB_Health->SetPercent(CurrentHealth / MaxHealth);
			}
		}
	}
}
