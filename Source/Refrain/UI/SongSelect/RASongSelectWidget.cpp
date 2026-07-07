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
#include "Components/AudioComponent.h"
#include "TimerManager.h"

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

void URASongSelectWidget::ActivateSongSelect()
{
	bIsPreviewAllowed = true;
	
	// 현재 리스트 뷰에서 선택된 곡이 있다면 즉시 재생
	if (SongListView)
	{
		UObject* SelectedItem = SongListView->GetSelectedItem();
		if (SelectedItem)
		{
			OnSongSelectionChanged(SelectedItem);
		}
	}
}

void URASongSelectWidget::OnBackClicked(int32 ButtonIndex)
{
	UE_LOG(LogTemp, Log, TEXT("URASongSelectWidget::OnBackClicked"));
	
	bIsPreviewAllowed = false;
	GetWorld()->GetTimerManager().ClearTimer(PreviewTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(PreviewRestartTimerHandle);

	if (PreviewAudioComponent)
	{
		PreviewAudioComponent->Stop();
	}

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

	bIsPreviewAllowed = false;
	GetWorld()->GetTimerManager().ClearTimer(PreviewTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(PreviewRestartTimerHandle);

	if (PreviewAudioComponent)
	{
		PreviewAudioComponent->Stop();
	}

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
	GetWorld()->GetTimerManager().ClearTimer(PreviewTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(PreviewRestartTimerHandle);
	if (PreviewAudioComponent)
	{
		PreviewAudioComponent->Stop();
	}

	// Item이 nullptr인 경우(선택 해제)도 발생할 수 있으므로 체크합니다.
	if (Item)
	{
		if (UMagicalMusicData* SelectedData = Cast<UMagicalMusicData>(Item))
		{
			CurrentSelectedSong = SelectedData;

			// UI 업데이트
			UpdateSongInfo(SelectedData);

			// 오디오 미리듣기 재생
			if (bIsPreviewAllowed)
			{
				StartPreviewAudio();
			}
		}
	}
}

void URASongSelectWidget::StartPreviewAudio()
{
	if (!bIsPreviewAllowed || !CurrentSelectedSong) return;

	if (CurrentSelectedSong->MusicSound.IsValid() || CurrentSelectedSong->MusicSound.LoadSynchronous())
	{
		PreviewAudioComponent = NewObject<UAudioComponent>(GetWorld());
		PreviewAudioComponent->SetSound(CurrentSelectedSong->MusicSound.Get());
		PreviewAudioComponent->bAutoDestroy = true;
		PreviewAudioComponent->bIsUISound = true;
		PreviewAudioComponent->bAllowSpatialization = false;
		PreviewAudioComponent->bAutoActivate = false; // 생성되자마자 자동 재생되지 않게 함 (Pop 방지)
		PreviewAudioComponent->SetVolumeMultiplier(1.0f); // 기본 볼륨은 1.0이어야 함!
		PreviewAudioComponent->RegisterComponentWithWorld(GetWorld());
		
		if (PreviewAudioComponent)
		{
			// FadeIn 함수가 내부적으로 Play()를 호출하며 0에서 1.0으로 서서히 볼륨을 올림
			PreviewAudioComponent->FadeIn(0.5f, 1.0f, CurrentSelectedSong->PreviewStartTime);
		}
		
		// 지정된 하이라이트 길이만큼만 재생 후 페이드 아웃 예약
		if (CurrentSelectedSong->PreviewDuration > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(PreviewTimerHandle, this, &URASongSelectWidget::OnPreviewFinished, CurrentSelectedSong->PreviewDuration, false);
		}
	}
}

void URASongSelectWidget::OnPreviewFinished()
{
	if (PreviewAudioComponent)
	{
		// 1.5초에 걸쳐 볼륨을 서서히 줄여서 끕니다.
		PreviewAudioComponent->FadeOut(1.5f, 0.0f);
	}

	// 페이드 아웃 진행 시간(1.5초) + 잠시 대기(0.5초) = 2.0초 뒤에 다시 미리듣기를 처음부터 루프 재생
	GetWorld()->GetTimerManager().SetTimer(PreviewRestartTimerHandle, this, &URASongSelectWidget::StartPreviewAudio, 2.0f, false);
}
