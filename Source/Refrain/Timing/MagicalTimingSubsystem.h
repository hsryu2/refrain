// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MagicalMusicData.h"
#include "Subsystems/WorldSubsystem.h"
#include "MagicalTimingSubsystem.generated.h"

class UAudioComponent;
class UQuartzClockHandle;
struct FStreamableHandle;
/**
 * 
 */
UCLASS()
class REFRAIN_API UMagicalTimingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UMagicalTimingSubsystem();
	
// 엔진 재정의 함수
protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	
	
public:
// 게임 로직 - 음악 재생
	UFUNCTION(BlueprintCallable, Category = "Magical|Control")
	bool SetMusicData(UMagicalMusicData* NewMusicData);
	
	UFUNCTION(BlueprintCallable, Category = "Magical|Control")
	bool StartMusic();
	
	UFUNCTION(BlueprintCallable, Category = "Magical|Control")
	bool StopMusic();

// Getter
	UFUNCTION(BlueprintCallable, Category = "Magical|Info")
	FORCEINLINE float GetBPM() { return MusicData ? MusicData->BPM : 0.f;}
	
	UFUNCTION(BlueprintCallable, Category = "Magical|Info")
	FORCEINLINE float GetSecondsPerBeat() { return MusicData ? 60.f / MusicData->BPM : 0.f;}

	UFUNCTION(BlueprintCallable, Category = "Magical|Info")
	float GetBeatProgress();
	
	
// 게임 로직 - 공격 시 판정
	// 목표 박과의 차이를 반환하는 함수 (점수 판정용)
	UFUNCTION(BlueprintCallable, Category = "Magical|Timing", meta = (CPP_Default_Quantization = "Beat"))
	float JudgeTiming(EQuartzCommandQuantization TargetQuantization = EQuartzCommandQuantization::Beat, float Multiplier = 1.f);
	
	// 다음 타격 타이밍까지 남은 시간을 반환하는 함수 (공격 모션 재생용)
	UFUNCTION(BlueprintCallable, Category = "Magical|Timing", meta = (CPP_Default_Quantization = "Beat"))
	float GetTimeUntilNextHit(float MinimumStartupDelay, EQuartzCommandQuantization TargetQuantization = EQuartzCommandQuantization::Beat, float Multiplier = 1.f);
	
	
protected:
// 재생 중인 음악 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
	TObjectPtr<UMagicalMusicData> MusicData;

	TSharedPtr<FStreamableHandle> MusicSoundLoadHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
	float MusicVolume = 1.f;
	
	
private:
// Quartz 시스템
	bool CreateQuartzClock();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music, meta = (AllowPrivateAccess = true))
	FName ClockName = TEXT("MagicalClock");
	
	UPROPERTY(Transient)
	TObjectPtr<UQuartzClockHandle> MusicClockHandle;
	
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> AudioComponent;
};
