// Fill out your copyright notice in the Description page of Project Settings.

#include "RAPauseMenuWidget.h"
#include "UI/MainMenu/RAMenuButtonWidget.h"
#include "UI/Settings/RAVolumeSettingsMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Player/RAPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void URAPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Resume)
	{
		Btn_Resume->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAPauseMenuWidget::UpdateHighlightPosition);
		Btn_Resume->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAPauseMenuWidget::OnResumeClicked);
	}
	if (Btn_Settings)
	{
		Btn_Settings->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAPauseMenuWidget::UpdateHighlightPosition);
		Btn_Settings->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAPauseMenuWidget::OnSettingsClicked);
	}
	if (Btn_MainMenu)
	{
		Btn_MainMenu->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAPauseMenuWidget::UpdateHighlightPosition);
		Btn_MainMenu->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAPauseMenuWidget::OnMainMenuClicked);
	}
	if (Btn_ExitGame)
	{
		Btn_ExitGame->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAPauseMenuWidget::UpdateHighlightPosition);
		Btn_ExitGame->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAPauseMenuWidget::OnExitGameClicked);
	}

	if (VolumeSettingsMenu)
	{
		VolumeSettingsMenu->OnSettingsMenuClosed.AddUniqueDynamic(this, &URAPauseMenuWidget::OnSettingsMenuClosed);
	}

	TargetTranslationY = SelectedIndex * MenuSpacing;
	CurrentTranslationY = TargetTranslationY;
}

void URAPauseMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (SelectionHighlight)
	{
		CurrentTranslationY = FMath::FInterpTo(CurrentTranslationY, TargetTranslationY, InDeltaTime, 12.0f);
		SelectionHighlight->SetRenderTranslation(FVector2D(0.0f, CurrentTranslationY));
	}
}

void URAPauseMenuWidget::UpdateHighlightPosition(int32 MenuIndex)
{
	if (SelectedIndex == MenuIndex)
	{
		return;
	}

	TArray<URAMenuButtonWidget*> MenuButtons = {Btn_Resume, Btn_Settings, Btn_MainMenu, Btn_ExitGame};
	
	if (MenuButtons.IsValidIndex(SelectedIndex) && MenuButtons[SelectedIndex])
	{
		MenuButtons[SelectedIndex]->OnSelectionStateChanged(false);
	}

	if (MenuButtons.IsValidIndex(MenuIndex) && MenuButtons[MenuIndex])
	{
		MenuButtons[MenuIndex]->OnSelectionStateChanged(true);
	}

	SelectedIndex = MenuIndex;
	TargetTranslationY = MenuIndex * MenuSpacing;
}

void URAPauseMenuWidget::OnResumeClicked(int32 MenuIndex)
{
	if (ARAPlayerController* PC = Cast<ARAPlayerController>(GetOwningPlayer()))
	{
		PC->TogglePauseMenu();
	}
}
void URAPauseMenuWidget::OnSettingsClicked(int32 MenuIndex)
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidgetIndex(1); // 1: 설정 메뉴
		
		if (SelectionHighlight)
		{
			SelectionHighlight->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void URAPauseMenuWidget::OnSettingsMenuClosed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidgetIndex(0); // 0: 기존 퍼즈 메뉴 버튼들
		
		if (SelectionHighlight)
		{
			SelectionHighlight->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void URAPauseMenuWidget::OnMainMenuClicked(int32 MenuIndex)
{
	if (!MainMenuLevel.IsNull())
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(*MainMenuLevel.GetAssetName()));
	}
	else
	{
		// 맵을 넣지 않았다면 경고 처리
		UE_LOG(LogTemp, Warning, TEXT("MainMenu Level 을 지정하지 않았습니다!"));
	}
}

void URAPauseMenuWidget::OnExitGameClicked(int32 MenuIndex)
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
