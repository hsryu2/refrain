// Fill out your copyright notice in the Description page of Project Settings.

#include "RAResultWidget.h"
#include "Refrain.h"
#include "UI/MainMenu/RAMenuButtonWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Player/RAPlayerState.h"
#include "RAResultRowWidget.h"

void URAResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BtnMainMenu)
	{
		BtnMainMenu->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAResultWidget::OnMainMenuClicked);
	}
	
	if (BtnRestart)
	{
		BtnRestart->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAResultWidget::OnRestartClicked);
	}
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ARAPlayerState* PS = PC->GetPlayerState<ARAPlayerState>())
		{
			if (TotalScoreRow) TotalScoreRow->SetValue(PS->TotalScore);
			if (PerfectCountRow) PerfectCountRow->SetValue(PS->PerfectCount);
			if (GoodCountRow) GoodCountRow->SetValue(PS->GoodCount);
			if (BadCountRow) BadCountRow->SetValue(PS->BadCount);
			if (MissCountRow) MissCountRow->SetValue(PS->MissCount);
			if (MaxHitsRow) MaxHitsRow->SetValue(PS->MaxHits);
		}
	}
}

void URAResultWidget::OnMainMenuClicked(int32 MenuIndex)
{
	if (!MainMenuLevel.IsNull())
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(*MainMenuLevel.GetAssetName()));
	}
	else
	{
		RA_LOG(LogRefrain, Warning, TEXT("MainMenu Level 을 지정하지 않았습니다!"));
	}
}

void URAResultWidget::OnRestartClicked(int32 MenuIndex)
{
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
}
