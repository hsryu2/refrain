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
	UFUNCTION(BlueprintCallable, Category = Music)
	bool SetMusicData(UMagicalMusicData* NewMusicData);
	
	UFUNCTION(BlueprintCallable, Category = Music)
	bool StartMusic();
	
	UFUNCTION(BlueprintCallable, Category = Music)
	bool StopMusic();
	
// 
	
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
	FName ClockName = TEXT("MusicClock");
	
	UPROPERTY(Transient)
	TObjectPtr<UQuartzClockHandle> MusicClockHandle;
	
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> AudioComponent;
};
