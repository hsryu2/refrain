// Fill out your copyright notice in the Description page of Project Settings.


#include "RAAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "RefrainGameplayTags.h"
#include "Player/RAPlayerState.h"

URAAttributeSet::URAAttributeSet() 
	: AttackPower(0.0f)
	, Health(0.0f)
	, MaxHealth(0.0f)
{
	
}

void URAAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(1.0f, NewValue);
	}
}

void URAAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetMaxHealthAttribute())
	{
		if (GetHealth() == 0.0f)
		{
			SetHealth(NewValue);
		}
		else if (GetHealth() > NewValue)
		{
			SetHealth(NewValue);
		}
	}
}

void URAAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float IncomingDamage = GetDamage();
		SetDamage(0.0f);
		
		const float NewHealth = FMath::Clamp(
			GetHealth() - IncomingDamage,
			0.0f,
			GetMaxHealth()
		);
		
		SetHealth(NewHealth);
		
		if (NewHealth <= 0.0f)
		{
			// 여기서 Dead state 태그 부여해야함.
		}
		else
		{
			// 피격 모션
			UAbilitySystemComponent* TargetASC = GetOwningAbilitySystemComponent();
			
			FGameplayEventData EventData;
			EventData.EventMagnitude = IncomingDamage;
			
			TargetASC->HandleGameplayEvent(RefrainGameplayTags::State_HitReact, &EventData);
		}
		
		if (ARAPlayerState* PlayerState = Cast<ARAPlayerState>(Data.Target.GetOwnerActor()))
		{
			PlayerState->ResetHits();
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Damage Applied: %.1f, HP: %.1f / %.1f"),
		IncomingDamage,
		GetHealth(),
		GetMaxHealth());
	}
}