// Fill out your copyright notice in the Description page of Project Settings.

#include "RAPauseMenuWidget.h"
#include "UI/MainMenu/RAMenuButtonWidget.h"
#include "UI/Settings/RAVolumeSettingsMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Player/RAPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

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

	// 다시 열렸을 때를 대비한 상태 초기화
	bIsCountingIn = false;
	
	if (MenuSwitcher) MenuSwitcher->SetVisibility(ESlateVisibility::Visible);
	if (SelectionHighlight) SelectionHighlight->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (CountInText) CountInText->SetVisibility(ESlateVisibility::Hidden);

	// 호버 애니메이션 상태 초기화
	TArray<URAMenuButtonWidget*> MenuButtons = {Btn_Resume, Btn_Settings, Btn_MainMenu, Btn_ExitGame};
	for (URAMenuButtonWidget* Btn : MenuButtons)
	{
		if (Btn) Btn->OnSelectionStateChanged(false);
	}
	
	SelectedIndex = -1;
	TargetTranslationY = 0.0f; // 애니메이션이 0부터 시작하게 하거나 원하는 초기 위치 지정
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

	// 카운트다운 로직
	if (bIsCountingIn)
	{
		CountInTime -= InDeltaTime;

		if (CountInTime > 0.0f)
		{
			if (CountInText)
			{
				int32 DisplayNumber = FMath::CeilToInt(CountInTime);
				CountInText->SetText(FText::AsNumber(DisplayNumber));
			}
		}
		else
		{
			// 카운트다운 종료! 게임 재개
			bIsCountingIn = false;
			if (CountInText)
			{
				CountInText->SetVisibility(ESlateVisibility::Hidden);
			}

			if (ARAPlayerController* PC = Cast<ARAPlayerController>(GetOwningPlayer()))
			{
				PC->ExecuteUnpause();
			}
		}
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

void URAPauseMenuWidget::StartCountIn()
{
	bIsCountingIn = true;
	CountInTime = 3.0f; // 3초 카운트인

	// 기존 버튼 숨기기
	if (MenuSwitcher)
	{
		MenuSwitcher->SetVisibility(ESlateVisibility::Hidden);
	}
	if (SelectionHighlight)
	{
		SelectionHighlight->SetVisibility(ESlateVisibility::Hidden);
	}

	// 카운트인 텍스트 보이기
	if (CountInText)
	{
		CountInText->SetVisibility(ESlateVisibility::HitTestInvisible);
		CountInText->SetText(FText::AsNumber(3));
	}
}

void URAPauseMenuWidget::CancelCountIn()
{
	bIsCountingIn = false;

	// 기존 메뉴 다시 보이기
	if (MenuSwitcher)
	{
		MenuSwitcher->SetVisibility(ESlateVisibility::Visible);
	}
	if (SelectionHighlight)
	{
		SelectionHighlight->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	// 카운트다운 텍스트 숨기기
	if (CountInText)
	{
		CountInText->SetVisibility(ESlateVisibility::Hidden);
	}
}
