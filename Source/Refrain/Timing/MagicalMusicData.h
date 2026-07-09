// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/QuartzQuantizationUtilities.h"
#include "MagicalMusicData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class REFRAIN_API UMagicalMusicData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Data")
	TSoftObjectPtr<USoundBase> MusicSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Metadata")
	FString SongTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Metadata")
	FString Artist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Metadata")
	TObjectPtr<class UTexture2D> JacketImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Data", meta = (ClampMin = "60.0", ClampMax = "240.0"))
	float BPM = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Data")
	int32 NumBeats = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Data")
	EQuartzTimeSignatureQuantization BeatType = EQuartzTimeSignatureQuantization::QuarterNote;

	FQuartzClockSettings ToQuartzClockSettings() const
	{
		FQuartzClockSettings Settings;
		Settings.TimeSignature.NumBeats = NumBeats;
		Settings.TimeSignature.BeatType = BeatType;
		return Settings;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Data")
	float StartOffset = 0.0f;

	/** @brief 리스트 뷰에서 선택 시 미리듣기 오디오가 시작될 시간 (초 단위) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Data")
	float PreviewStartTime = 0.0f;

	/** @brief 미리듣기가 재생될 길이 (초 단위). 0보다 크면 지정된 시간 후 페이드 아웃됩니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Data")
	float PreviewDuration = 15.0f;
};
