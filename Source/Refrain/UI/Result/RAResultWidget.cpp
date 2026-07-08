// Fill out your copyright notice in the Description page of Project Settings.

#include "RAResultWidget.h"
#include "Refrain.h"
#include "UI/MainMenu/RAMenuButtonWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Player/RAPlayerState.h"
#include "RAResultRowWidget.h"

void URAResultWidget::SetScoreVisibility(bool bShow)
{
	ESlateVisibility NewVisibility = bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	
	if (TotalScoreRow) TotalScoreRow->SetVisibility(NewVisibility);
	if (PerfectCountRow) PerfectCountRow->SetVisibility(NewVisibility);
	if (GoodCountRow) GoodCountRow->SetVisibility(NewVisibility);
	if (BadCountRow) BadCountRow->SetVisibility(NewVisibility);
	if (MaxHitsRow) MaxHitsRow->SetVisibility(NewVisibility);
}

void URAResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BtnMainMenu)
	{
		BtnMainMenu->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAResultWidget::OnMainMenuClicked);
		BtnMainMenu->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAResultWidget::UpdateHighlightPosition);
		BtnMainMenu->OnSelectionStateChanged(false);
	}
	
	if (BtnRestart)
	{
		BtnRestart->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAResultWidget::OnRestartClicked);
		BtnRestart->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAResultWidget::UpdateHighlightPosition);
		BtnRestart->OnSelectionStateChanged(false);
	}
	
	SelectedIndex = -1;
	
	if (bIsGameOver)
	{
		SetScoreVisibility(false);
		// 블루프린트에서 LabelTitle 위젯이 있다면 "GAME OVER"로 텍스트를 바꾸는 건 블루프린트 단에서 하거나 
		// 여기서 바인딩해서 처리할 수 있습니다.
	}
	else
	{
		SetScoreVisibility(true);
		
		// 점수 계산 및 표시
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (ARAPlayerState* PS = PC->GetPlayerState<ARAPlayerState>())
			{
				if (TotalScoreRow) TotalScoreRow->SetValue(PS->TotalScore);
				if (PerfectCountRow) PerfectCountRow->SetValue(PS->PerfectCount);
				if (GoodCountRow) GoodCountRow->SetValue(PS->GoodCount);
				if (BadCountRow) BadCountRow->SetValue(PS->BadCount);
				if (MaxHitsRow) MaxHitsRow->SetValue(PS->MaxHits);
			}
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

void URAResultWidget::UpdateHighlightPosition(int32 MenuIndex)
{
	if (SelectedIndex == MenuIndex)
	{
		return;
	}

	// 블루프린트에서 MainMenu 버튼의 MenuIndex를 0, Restart 버튼을 1로 세팅했다고 가정합니다.
	// 만약 인덱스가 다르게 세팅되어 있다면 배열 순서를 바꾸거나 크기를 늘려야 할 수 있습니다.
	TArray<URAMenuButtonWidget*> MenuButtons = {BtnMainMenu, BtnRestart};
	
	if (MenuButtons.IsValidIndex(SelectedIndex) && MenuButtons[SelectedIndex])
	{
		MenuButtons[SelectedIndex]->OnSelectionStateChanged(false);
	}

	if (MenuButtons.IsValidIndex(MenuIndex) && MenuButtons[MenuIndex])
	{
		MenuButtons[MenuIndex]->OnSelectionStateChanged(true);
	}

	SelectedIndex = MenuIndex;
}
