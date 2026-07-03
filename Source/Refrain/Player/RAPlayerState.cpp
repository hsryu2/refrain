// Fill out your copyright notice in the Description page of Project Settings.


#include "RAPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Refrain/GA/Attribute/RAAttributeSet.h"
//#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayEffect.h"
#include "Refrain.h"

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

// ENum으로 받은 설정 점수 등록 및 Hit증가
void ARAPlayerState::RegisterJudgement(ERAHitJudgement Judgement)
{
	int32 BaseScore = 0;
	switch (Judgement)
	{
		case ERAHitJudgement::Perfect:
			BaseScore = 100;
			CurrentHits++;
			break;
		case ERAHitJudgement::Good:
			BaseScore = 50;
			CurrentHits++;
			break;
		case ERAHitJudgement::Bad:
			BaseScore = 25;
			CurrentHits++;
			break;
		case ERAHitJudgement::Miss:
			BaseScore = 0;
			break;
		default:
			break;
	}
	MaxHits = FMath::Max(MaxHits, CurrentHits);
	const int32 AddedScore = FMath::RoundToInt(static_cast<float>(BaseScore) * GetHitsMultiplier());
	TotalScore += AddedScore;
	UE_LOG(LogTemp, Log, TEXT("TotalScore : %d, CurrentCombo : %d"), TotalScore, CurrentHits);
	
	OnScoreUpdated.Broadcast(Judgement, AddedScore, TotalScore, CurrentHits);
}

float ARAPlayerState::GetHitsMultiplier() const
{
	if (CurrentHits >= 50)
	{
		return 1.5f;
	}
	if (CurrentHits >= 30)
	{
		return 1.2f;
	}
	if (CurrentHits >= 10)
	{
		return 1.1f;
	}
	return 1.0f;
}

void ARAPlayerState::ResetHits()
{
	CurrentHits = 0;
}

class UAbilitySystemComponent* ARAPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
