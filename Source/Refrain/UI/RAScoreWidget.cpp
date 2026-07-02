// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RAScoreWidget.h"

void URAScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}
	
	ARAPlayerState* RAPlayerState = PC->GetPlayerState<ARAPlayerState>();
	if (!RAPlayerState)
	{
		return;
	}
	
	RAPlayerState->OnScoreUpdated.AddDynamic(this, &URAScoreWidget::HandleScoreUpdated);
	
}

void URAScoreWidget::HandleScoreUpdated(ERAHitJudgement Judgement, int32 AddedScore, int32 NewTotalScore,
	int32 NewCombo)
{
}
