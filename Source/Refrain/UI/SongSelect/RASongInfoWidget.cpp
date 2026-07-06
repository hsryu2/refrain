// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SongSelect/RASongInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Timing/MagicalMusicData.h"

void URASongInfoWidget::UpdateSongInfo(UMagicalMusicData* InSongData)
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
		ImgSongJacket->SetBrushFromTexture(InSongData->JacketImage);
	}
}
