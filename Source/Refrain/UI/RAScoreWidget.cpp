// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RAScoreWidget.h"
#include "Components/TextBlock.h"

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
	// 총 점수 갱신
	if (TotalScoreText)
	{
		TotalScoreText->SetText(FText::AsNumber(NewTotalScore));
	}

	// 콤보 갱신 (0이면 숨기기)
	if (ComboText)
	{
		if (NewCombo > 0)
		{
			ComboText->SetVisibility(ESlateVisibility::Visible);
			ComboText->SetText(FText::FromString(FString::Printf(TEXT("%d Hits!"), NewCombo)));
		}
		else
		{
			ComboText->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// 판정 갱신
	if (JudgementText)
	{
		FString JudgementString;
		switch (Judgement)
		{
			case ERAHitJudgement::Perfect: JudgementString = TEXT("PERFECT"); break;
			case ERAHitJudgement::Good:    JudgementString = TEXT("GOOD"); break;
			case ERAHitJudgement::Bad:     JudgementString = TEXT("BAD"); break;
			case ERAHitJudgement::Miss:    JudgementString = TEXT("MISS"); break;
		}
		JudgementText->SetText(FText::FromString(JudgementString));
	}
}
