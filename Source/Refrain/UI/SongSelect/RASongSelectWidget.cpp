// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SongSelect/RASongSelectWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ListView.h"

#include "Timing/MagicalMusicData.h"
#include "UI/MainMenu/RAMenuButtonWidget.h"

void URASongSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 바인딩된 버튼들에 이벤트 리스너 등록
	if (BtnBack)
	{
		BtnBack->OnMenuButtonClickedEvent.AddDynamic(this, &URASongSelectWidget::OnBackClicked);
	}
	
	if (BtnSettings)
	{
		BtnSettings->OnMenuButtonClickedEvent.AddDynamic(this, &URASongSelectWidget::OnSettingsClicked);
	}
	
	if (BtnCategory)
	{
		BtnCategory->OnMenuButtonClickedEvent.AddDynamic(this, &URASongSelectWidget::OnCategoryClicked);
	}
	
	if (BtnPlay)
	{
		BtnPlay->OnMenuButtonClickedEvent.AddDynamic(this, &URASongSelectWidget::OnPlayClicked);
	}
	
	if (SongListView)
	{
		SongListView->OnItemSelectionChanged().AddUObject(this, &URASongSelectWidget::OnSongSelectionChanged);
	}
}

void URASongSelectWidget::UpdateSongInfo(UMagicalMusicData* InSongData)
{
	if (!InSongData) return;

	if (TxtSongTitle)
	{
		TxtSongTitle->SetText(FText::FromString(InSongData->SongTitle));
	}

	if (TxtArtist)
	{
		TxtArtist->SetText(FText::FromString(InSongData->Artist));
	}

	if (TxtBPM)
	{
		FString BPMStr = FString::Printf(TEXT("BPM: %.0f"), InSongData->BPM);
		TxtBPM->SetText(FText::FromString(BPMStr));
	}

	if (ImgSongJacket && InSongData->JacketImage)
	{
		// 텍스처를 브러시에 세팅 (UMG)
		ImgSongJacket->SetBrushFromTexture(InSongData->JacketImage);
	}
}

void URASongSelectWidget::OnBackClicked(int32 ButtonIndex)
{
	// TODO: 메인 메뉴로 돌아가기 등 뒤로가기 로직 구현
	UE_LOG(LogTemp, Log, TEXT("URASongSelectWidget::OnBackClicked"));
}

void URASongSelectWidget::OnSettingsClicked(int32 ButtonIndex)
{
	// TODO: 설정 팝업 호출
	UE_LOG(LogTemp, Log, TEXT("URASongSelectWidget::OnSettingsClicked"));
}

void URASongSelectWidget::OnCategoryClicked(int32 ButtonIndex)
{
	// TODO: 카테고리(ALL/NEW/FAVORITE/LV) 전환 로직
	UE_LOG(LogTemp, Log, TEXT("URASongSelectWidget::OnCategoryClicked"));
}

void URASongSelectWidget::OnPlayClicked(int32 ButtonIndex)
{
	// TODO: 선택된 곡으로 게임 씬 전환 등 시작 로직 구현
	UE_LOG(LogTemp, Log, TEXT("URASongSelectWidget::OnPlayClicked"));
}

void URASongSelectWidget::OnSongSelectionChanged(UObject* Item)
{
	// Item이 nullptr인 경우(선택 해제)도 발생할 수 있으므로 체크합니다.
	if (Item)
	{
		UMagicalMusicData* SelectedData = Cast<UMagicalMusicData>(Item);
		if (SelectedData)
		{
			// ListView에서 선택된 데이터를 메인 UI에 업데이트
			UpdateSongInfo(SelectedData);
		}
	}
}
