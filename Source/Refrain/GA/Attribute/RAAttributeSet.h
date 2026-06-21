// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RAAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class REFRAIN_API URAAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	URAAttributeSet();
	
	ATTRIBUTE_ACCESSORS(URAAttributeSet, AttackPower);
	ATTRIBUTE_ACCESSORS(URAAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(URAAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(URAAttributeSet, Damage);
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
protected:
	UPROPERTY(BlueprintReadOnly, Category="Attack", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData AttackPower;
	
	UPROPERTY()
	FGameplayAttributeData Health;
	
	UPROPERTY()
	FGameplayAttributeData MaxHealth;
	
	UPROPERTY()
	FGameplayAttributeData Damage;
	
	
	
	
};
