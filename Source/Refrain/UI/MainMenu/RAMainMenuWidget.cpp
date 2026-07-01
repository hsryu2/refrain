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

void URAMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 호버 이벤트 바인딩
	if (Btn_Continue)
	{
		Btn_Continue->OnMenuButtonHoveredEvent.AddDynamic(this, &URAMainMenuWidget::UpdateHighlightPosition);
	}
	if (Btn_NewGame) 
	{
		Btn_NewGame->OnMenuButtonHoveredEvent.AddDynamic(this, &URAMainMenuWidget::UpdateHighlightPosition);
		Btn_NewGame->OnMenuButtonClickedEvent.AddDynamic(this, &URAMainMenuWidget::OnNewGameClicked);
	}
	if (Btn_Settings) 
	{
		Btn_Settings->OnMenuButtonHoveredEvent.AddDynamic(this, &URAMainMenuWidget::UpdateHighlightPosition);
		Btn_Settings->OnMenuButtonClickedEvent.AddDynamic(this, &URAMainMenuWidget::OnSettingsClicked);
	}
	if (Btn_Credits)
	{
		Btn_Credits->OnMenuButtonHoveredEvent.AddDynamic(this, &URAMainMenuWidget::UpdateHighlightPosition);
	}
	if (Btn_ExitGame) 
	{
		Btn_ExitGame->OnMenuButtonHoveredEvent.AddDynamic(this, &URAMainMenuWidget::UpdateHighlightPosition);
		
		// 게임 종료 버튼 클릭 이벤트 바인딩!
		Btn_ExitGame->OnMenuButtonClickedEvent.AddDynamic(this, &URAMainMenuWidget::OnExitGameClicked);
	}

	if (VolumeSettingsMenu)
	{
		VolumeSettingsMenu->OnSettingsMenuClosed.AddDynamic(this, &URAMainMenuWidget::OnSettingsMenuClosed);
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

	TArray<URAMenuButtonWidget*> MenuButtons = {Btn_Continue, Btn_NewGame, Btn_Settings, Btn_Credits, Btn_ExitGame};
	
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
	if (!NewGameLevel.IsNull())
	{
	    UGameplayStatics::OpenLevel(GetWorld(), FName(*NewGameLevel.GetAssetName()));
	}
	else
	{
		// 맵을 넣지 않았다면 경고 처리
		RA_LOG(LogRefrain, Warning, TEXT("Game Level이 지정되지 않았습니다 !"));
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

void URAMainMenuWidget::OnExitGameClicked(int32 MenuIndex)
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
