// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGameMode.h"
#include "Player/RAPlayerController.h"
#include "Player/RAPlayerState.h"
#include "Game/RAGameSessionSubsystem.h"
#include "Timing/MagicalTimingSubsystem.h"
#include "Kismet/GameplayStatics.h"

ARAGameMode::ARAGameMode()
{
	PlayerControllerClass = ARAPlayerController::StaticClass();
	PlayerStateClass = ARAPlayerState::StaticClass();
}

void ARAGameMode::StartPlay()
{
	Super::StartPlay();

	// 세션 서브시스템에서 선택한 곡 가져오기
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (URAGameSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<URAGameSessionSubsystem>())
		{
			if (SessionSubsystem->SelectedSong)
			{
				// 타이밍 서브시스템에 곡 세팅 및 재생 시작
				if (UMagicalTimingSubsystem* TimingSubsystem = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>())
				{
					TimingSubsystem->SetMusicData(SessionSubsystem->SelectedSong);
					TimingSubsystem->StartMusic();
					UE_LOG(LogTemp, Log, TEXT("RAGameMode: Started playing %s"), *SessionSubsystem->SelectedSong->SongTitle);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("RAGameMode: No SelectedSong found in GameSessionSubsystem."));
			}
		}
	}
}
