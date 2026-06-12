// Fill out your copyright notice in the Description page of Project Settings.


#include "RAPlayerState.h"

#include "AbilitySystemComponent.h"

ARAPlayerState::ARAPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	
}

class UAbilitySystemComponent* ARAPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
