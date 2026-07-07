// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/RASaveScoreSubsystem.h"

#include "RASaveScore.h"
#include "Chaos/ChaosPerfTest.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Refrain.h"

void URASaveScoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 게임이 켜질 때 기존 세이브 파일이 있으면 로드.
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveScoreInstance = Cast<URASaveScore>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}
	
	// 세이브 파일이 없거나 로드 실패 했다면 빈 세이브 객체 생성.
	if (SaveScoreInstance == nullptr)
	{
		SaveScoreInstance = Cast<URASaveScore>(UGameplayStatics::CreateSaveGameObject(URASaveScore::StaticClass()));
	}
}

void URASaveScoreSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

int32 URASaveScoreSubsystem::GetSongHighScore(FName SongID) const
{
	// Map에 점수가 있으면 반환.
	if (SaveScoreInstance && SaveScoreInstance->SongHighScores.Contains(SongID))
	{
		return SaveScoreInstance->SongHighScores[SongID];
	}
	// 없으면 0점.
	return 0;
}

void URASaveScoreSubsystem::UpdateAndSaveSongHighScore(FName SongID, int32 CurrentScore)
{
	if (!SaveScoreInstance) return;
	
	bool bShouldSave = false;
	
	// 이미 기록된 곡인지 확인.
	if (SaveScoreInstance->SongHighScores.Contains(SongID))
	{
		// 기존 점수보다 높으면 갱신
		if (CurrentScore > SaveScoreInstance->SongHighScores[SongID])
		{
			SaveScoreInstance->SongHighScores[SongID] = CurrentScore;
			bShouldSave = true;
		}
	}
	else
	{
		// 기록이 없는 노래라면 새로 추가.
		SaveScoreInstance->SongHighScores.Add(SongID, CurrentScore);
		bShouldSave = true;
	}
	
	// 점수 변동이 있으면 세이브
	if (bShouldSave)
	{
		UGameplayStatics::SaveGameToSlot(SaveScoreInstance, TEXT("RAScoreSlot"), 0);
		RA_LOG(LogRefrain, Log, TEXT("Song High Score Updated: %s, %d"), *SongID.ToString(), CurrentScore);
	}
}
