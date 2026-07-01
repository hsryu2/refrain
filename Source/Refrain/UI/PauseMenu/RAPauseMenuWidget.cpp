// Fill out your copyright notice in the Description page of Project Settings.

#include "RAPauseMenuWidget.h"
#include "UI/MainMenu/RAMenuButtonWidget.h"
#include "Player/RAPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void URAPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Resume)
	{
		Btn_Resume->OnMenuButtonHoveredEvent.AddDynamic(this, &URAPauseMenuWidget::UpdateHighlightPosition);
		Btn_Resume->OnMenuButtonClickedEvent.AddDynamic(this, &URAPauseMenuWidget::OnResumeClicked);
	}
	if (Btn_Settings)
	{
		Btn_Settings->OnMenuButtonHoveredEvent.AddDynamic(this, &URAPauseMenuWidget::UpdateHighlightPosition);
		Btn_Settings->OnMenuButtonClickedEvent.AddDynamic(this, &URAPauseMenuWidget::OnSettingsClicked);
	}
	if (Btn_MainMenu)
	{
		Btn_MainMenu->OnMenuButtonHoveredEvent.AddDynamic(this, &URAPauseMenuWidget::UpdateHighlightPosition);
		Btn_MainMenu->OnMenuButtonClickedEvent.AddDynamic(this, &URAPauseMenuWidget::OnMainMenuClicked);
	}
	if (Btn_ExitGame)
	{
		Btn_ExitGame->OnMenuButtonHoveredEvent.AddDynamic(this, &URAPauseMenuWidget::UpdateHighlightPosition);
		Btn_ExitGame->OnMenuButtonClickedEvent.AddDynamic(this, &URAPauseMenuWidget::OnExitGameClicked);
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
	if (SettingsMenuClass)
	{
		if (!SettingsMenuInstance)
		{
			SettingsMenuInstance = CreateWidget<UUserWidget>(GetOwningPlayer(), SettingsMenuClass);
		}

		if (SettingsMenuInstance && !SettingsMenuInstance->IsInViewport())
		{
			SettingsMenuInstance->AddToViewport(10);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Settings Menu Class is not set."));
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
