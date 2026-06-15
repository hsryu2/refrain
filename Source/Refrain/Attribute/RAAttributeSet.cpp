// Fill out your copyright notice in the Description page of Project Settings.


#include "RAAttributeSet.h"

URAAttributeSet::URAAttributeSet() : AttackRate(100.0f)
{
	
}

void URAAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void URAAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}
