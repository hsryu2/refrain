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
};
