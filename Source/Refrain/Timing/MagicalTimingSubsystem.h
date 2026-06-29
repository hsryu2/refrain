// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MagicalMusicData.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "Sound/QuartzQuantizationUtilities.h"
#include "Subsystems/WorldSubsystem.h"
#include "MagicalTimingSubsystem.generated.h"

class USoundBase;
class UAudioComponent;
struct FStreamableHandle;
/**
 * 
 */
UCLASS()
class REFRAIN_API UMagicalTimingSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UMagicalTimingSubsystem();
	
// 엔진 재정의 함수
protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	
// 게임 로직 - 음악 재생
public:
	UFUNCTION(BlueprintCallable, Category = "Magical|Control")
	bool SetMusicData(UMagicalMusicData* NewMusicData);
	
	UFUNCTION(BlueprintCallable, Category = "Magical|Control")
	bool StartMusic();
	
	UFUNCTION(BlueprintCallable, Category = "Magical|Control")
	bool StopMusic();
	
// 게임 로직 - 효과음 재생
public:
	UFUNCTION(BlueprintCallable, Category = "Magical|Control")
	bool PlaySFXQuantized(USoundBase* InSound, EQuartzCommandQuantization InQuantization = EQuartzCommandQuantization::Beat, float InMultiplier = 1.f);

// Getter
public:
	UFUNCTION(BlueprintCallable, Category = "Magical|Info")
	float GetBPM() { return MusicData ? MusicData->BPM : 0.f;}
	
	UFUNCTION(BlueprintCallable, Category = "Magical|Info")
	float GetSecondsPerBeat() { return MusicData ? 60.f / MusicData->BPM : 0.f;}

	// 현재 박자 내의 진행 정도를 0.f ~ 1.f 사이의 값으로 반환하는 함수 
	UFUNCTION(BlueprintCallable, Category = "Magical|Info")
	float GetBeatProgress();

	UFUNCTION(BlueprintCallable, Category = "Magical|Info")
	bool IsMusicPlaying();

	UFUNCTION(BlueprintCallable, Category = "Magical|Quratz")
	UQuartzClockHandle* GetMusicClockHandle() const { return MusicClockHandle;}
	
// 게임 로직 - 공격 시 판정
public:
	// 목표 박과의 차이를 반환하는 함수 (점수 판정용)
	UFUNCTION(BlueprintCallable, Category = "Magical|Timing", meta = (CPP_Default_Quantization = "Beat"))
	float JudgeTiming(EQuartzCommandQuantization TargetQuantization = EQuartzCommandQuantization::Beat, float Multiplier = 1.f);
	
	// 다음 타격 타이밍까지 남은 시간을 반환하는 함수 (공격 모션 재생용)
	UFUNCTION(BlueprintCallable, Category = "Magical|Timing", meta = (CPP_Default_Quantization = "Beat"))
	float GetTimeUntilNextHit(float MinimumStartupDelay, EQuartzCommandQuantization TargetQuantization = EQuartzCommandQuantization::Beat, float Multiplier = 1.f);

// 내부 로직에 필요한 함수
private:
	bool CreateQuartzClock();
	
// 재생 중인 음악 관련 변수
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
	TObjectPtr<UMagicalMusicData> MusicData;

	TSharedPtr<FStreamableHandle> MusicSoundLoadHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
	float MusicVolume = 1.f;
	
	
// Quartz 및 음악 재생 시스템 관련 변수
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music, meta = (AllowPrivateAccess = true))
	FName ClockName = TEXT("MagicalClock");
	
	UPROPERTY(Transient)
	TObjectPtr<UQuartzClockHandle> MusicClockHandle;
	
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> MusicAudioComponent;
	
private:
	// 머티리얼 파라미터 컬렉션 포인터 저장
	UPROPERTY(VisibleAnywhere, Category = "Magical|Material")
	TObjectPtr<UMaterialParameterCollectionInstance> MagicalTimingMPCInstance;
};
