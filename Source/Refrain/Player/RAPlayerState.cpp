// Fill out your copyright notice in the Description page of Project Settings.


#include "RAPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Refrain/GA/Attribute/RAAttributeSet.h"
//#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayEffect.h"

ARAPlayerState::ARAPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	// 멀티 전용으로 보임.
	//ASC->SetIsReplicated(true);
	//ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<URAAttributeSet>(TEXT("AttributeSet"));
}

void ARAPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (ASC && InitStatEffect)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddInstigator(GetPawn(), GetPawn());

		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InitStatEffect, 1.0f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

class UAbilitySystemComponent* ARAPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}