// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SongSelect/RASongInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Timing/MagicalMusicData.h"
#include "SaveGame/RASaveScoreSubsystem.h"
#include "Engine/GameInstance.h"
#include "UI/Result/RAResultRowWidget.h"

void URASongInfoWidget::UpdateSongInfo(UMagicalMusicData* InSongData)
{
	// 방어 코드
	if (!InSongData) return;

	if (TxtSongTitle)
	{
		TxtSongTitle->SetText(FText::FromString(InSongData->SongTitle));
	}

	if (TxtArtist)
	{
		TxtArtist->SetText(FText::FromString(InSongData->Artist));
	}

	// BPM 정보를 위한 데이터 가공
	if (TxtBPM)
	{
		FString BPMStr = FString::Printf(TEXT("BPM: %.0f"), InSongData->BPM);
		TxtBPM->SetText(FText::FromString(BPMStr));
	}

	if (ImgSongJacket && InSongData->JacketImage)
	{
		ImgSongJacket->SetBrushFromTexture(InSongData->JacketImage);
	}
	
	// 서브시스템에서 점수들 불러오기
	if (HighScoreRow)
	{
		int32 FoundHighScore = 0;
		if (URASaveScoreSubsystem* ScoreSubsystem = GetGameInstance()->GetSubsystem<URASaveScoreSubsystem>())
		{
			FoundHighScore = ScoreSubsystem->GetSongHighScore(FName(*InSongData->SongTitle));
		}
		HighScoreRow->SetValue(FoundHighScore);
	}
}
