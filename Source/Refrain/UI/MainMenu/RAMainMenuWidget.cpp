// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/RAMainMenuWidget.h"

#include "Refrain.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

#include "Math/UnrealMathUtility.h"

#include "UI/MainMenu/RAMenuButtonWidget.h"
#include "UI/Settings/RAVolumeSettingsMenuWidget.h"
#include "UI/SongSelect/RASongSelectWidget.h"

void URAMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 호버 이벤트 바인딩
	if (BtnContinue)
	{
		BtnContinue->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAMainMenuWidget::UpdateHighlightPosition);
	}
	if (BtnNewGame) 
	{
		BtnNewGame->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAMainMenuWidget::UpdateHighlightPosition);
		BtnNewGame->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAMainMenuWidget::OnNewGameClicked);
	}
	if (BtnSettings) 
	{
		BtnSettings->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAMainMenuWidget::UpdateHighlightPosition);
		BtnSettings->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAMainMenuWidget::OnSettingsClicked);
	}
	if (BtnCredits)
	{
		BtnCredits->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAMainMenuWidget::UpdateHighlightPosition);
	}
	if (BtnExitGame) 
	{
		BtnExitGame->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAMainMenuWidget::UpdateHighlightPosition);
		
		// 게임 종료 버튼 클릭 이벤트 바인딩!
		BtnExitGame->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAMainMenuWidget::OnExitGameClicked);
	}

	if (VolumeSettingsMenu)
	{
		VolumeSettingsMenu->OnSettingsMenuClosed.AddUniqueDynamic(this, &URAMainMenuWidget::OnSettingsMenuClosed);
	}

	if (SongSelectMenu)
	{
		SongSelectMenu->OnSongSelectClosed.AddUniqueDynamic(this, &URAMainMenuWidget::OnSongSelectMenuClosed);
	}

	// 초기 위치 설정 (선택된 인덱스 기준)
	TargetTranslationY = SelectedIndex * MenuSpacing;
	CurrentTranslationY = TargetTranslationY;
}

void URAMainMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 하이라이트 위젯 부드러운 이동 (FInterpTo 사용)
	if (SelectionHighlight)
	{
		CurrentTranslationY = FMath::FInterpTo(CurrentTranslationY, TargetTranslationY, InDeltaTime, 12.0f);
		SelectionHighlight->SetRenderTranslation(FVector2D(0.0f, CurrentTranslationY));
	}
}

void URAMainMenuWidget::UpdateHighlightPosition(int32 MenuIndex)
{
	// 같은 버튼에 다시 마우스를 올린 거라면 무시
	if (SelectedIndex == MenuIndex)
	{
		return;
	}

	TArray<URAMenuButtonWidget*> MenuButtons = {BtnContinue, BtnNewGame, BtnSettings, BtnCredits, BtnExitGame};
	
	// 1. 이전에 선택되어 있던 버튼만 찾아서 애니메이션 끄기 (역재생)
	if (MenuButtons.IsValidIndex(SelectedIndex) && MenuButtons[SelectedIndex])
	{
		MenuButtons[SelectedIndex]->OnSelectionStateChanged(false);
	}

	// 2. 방금 새로 마우스가 올라간 버튼만 애니메이션 켜기 (정재생)
	if (MenuButtons.IsValidIndex(MenuIndex) && MenuButtons[MenuIndex])
	{
		MenuButtons[MenuIndex]->OnSelectionStateChanged(true);
	}

	// 3. 사다리꼴의 이동 및 현재 인덱스 갱신
	SelectedIndex = MenuIndex;
	TargetTranslationY = MenuIndex * MenuSpacing;
}

void URAMainMenuWidget::OnNewGameClicked(int32 MenuIndex)
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidgetIndex(2); // 2: 곡 선택 메뉴
		
		if (SelectionHighlight)
		{
			SelectionHighlight->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void URAMainMenuWidget::OnSettingsClicked(int32 MenuIndex)
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

void URAMainMenuWidget::OnSettingsMenuClosed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidgetIndex(0); // 0: 메인 메뉴 버튼들
		
		if (SelectionHighlight)
		{
			SelectionHighlight->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void URAMainMenuWidget::OnSongSelectMenuClosed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidgetIndex(0); // 0: 메인 메뉴 버튼들
		
		if (SelectionHighlight)
		{
			SelectionHighlight->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void URAMainMenuWidget::OnExitGameClicked(int32 MenuIndex)
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
