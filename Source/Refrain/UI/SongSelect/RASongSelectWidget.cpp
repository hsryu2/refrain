// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SongSelect/RASongSelectWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ListView.h"

#include "Timing/MagicalMusicData.h"
#include "UI/MainMenu/RAMenuButtonWidget.h"
#include "UI/SongSelect/RASongInfoWidget.h"
#include "Game/RAGameSessionSubsystem.h"
#include "Kismet/GameplayStatics.h"

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

	// 뷰 초기화
	InitSongList();
}

void URASongSelectWidget::InitSongList()
{
	if (!SongListView) return;

	// 기존 아이템 지우기
	SongListView->ClearListItems();

	// 배열에 추가된 곡 데이터 리스트 뷰에 등록
	for (UMagicalMusicData* SongData : AvailableSongs)
	{
		if (SongData)
		{
			SongListView->AddItem(SongData);
		}
	}

	// 곡이 하나라도 있다면 첫 번째 곡을 기본으로 선택
	if (AvailableSongs.Num() > 0 && AvailableSongs[0] != nullptr)
	{
		SongListView->SetSelectedItem(AvailableSongs[0]);
	}
}

void URASongSelectWidget::UpdateSongInfo(UMagicalMusicData* InSongData)
{
	if (!InSongData) return;

	if (SongInfoPanel)
	{
		SongInfoPanel->UpdateSongInfo(InSongData);
	}
}

void URASongSelectWidget::OnBackClicked(int32 ButtonIndex)
{
	// TODO: 메인 메뉴로 돌아가기 등 뒤로가기 로직 구현
	UE_LOG(LogTemp, Log, TEXT("URASongSelectWidget::OnBackClicked"));
	OnSongSelectClosed.Broadcast();
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
	UE_LOG(LogTemp, Log, TEXT("URASongSelectWidget::OnPlayClicked"));

	if (CurrentSelectedSong)
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			if (URAGameSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<URAGameSessionSubsystem>())
			{
				SessionSubsystem->SelectedSong = CurrentSelectedSong;
			}
		}

		if (!GameplayLevel.IsNull())
		{
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, GameplayLevel);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameplayLevel is null. Please set it in WBP_SongSelect."));
		}
	}
}

void URASongSelectWidget::OnSongSelectionChanged(UObject* Item)
{
	// Item이 nullptr인 경우(선택 해제)도 발생할 수 있으므로 체크합니다.
	if (Item)
	{
		UMagicalMusicData* SelectedData = Cast<UMagicalMusicData>(Item);
		if (SelectedData)
		{
			CurrentSelectedSong = SelectedData;
			// ListView에서 선택된 데이터를 메인 UI에 업데이트
			UpdateSongInfo(SelectedData);
		}
	}
}
