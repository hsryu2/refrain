// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicalTimingSubsystem.h"

#include "Components/AudioComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "Quartz/QuartzSubsystem.h"
#include "Refrain/Refrain.h"
#include "Sound/SoundBase.h"

UMagicalTimingSubsystem::UMagicalTimingSubsystem()
{
}

bool UMagicalTimingSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// return Super::DoesSupportWorldType(WorldType);
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

bool UMagicalTimingSubsystem::SetMusicData(UMagicalMusicData* NewMusicData)
{
	if (!IsValid(NewMusicData))
	{
		RA_LOG(LogRefrain, Error, TEXT("MusicData is invalid"));
		return false;
	}
	
	// MusicData 설정
	MusicData = NewMusicData;
	
	// 기존 로드 취소
	if (MusicSoundLoadHandle.IsValid())
	{
		MusicSoundLoadHandle->CancelHandle();
	}
	MusicSoundLoadHandle.Reset();

	if (MusicData->MusicSound.IsNull())
	{
		RA_LOG(LogRefrain, Error, TEXT("MusicSound is not set"));
		return false;
	}

	// 이미 로드되어 있는 경우
	if (MusicData->MusicSound.IsValid())
	{
		return true;
	}

	// 비동기 로드
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	MusicSoundLoadHandle = StreamableManager.RequestAsyncLoad(MusicData->MusicSound.ToSoftObjectPath());
	return MusicSoundLoadHandle.IsValid();
}

bool UMagicalTimingSubsystem::StartMusic()
{
	if (!IsValid(MusicData))
	{
		RA_LOG(LogRefrain, Error, TEXT("MusicData is invalid"));
		return false;
	}
	
	// 기존 핸들 초기화
	StopMusic();

	// 음악 사운드
	USoundBase* MusicSound = MusicData->MusicSound.Get();
	if (!IsValid(MusicSound))
	{
		MusicSound = MusicData->MusicSound.LoadSynchronous();
	}
	if (!IsValid(MusicSound))
	{
		RA_LOG(LogRefrain, Error, TEXT("MusicSound is invalid"));
		return false;
	}
	
	// Clock 생성
	if (!CreateQuartzClock())
	{
		RA_LOG(LogRefrain, Error, TEXT("Failed to create Quartz clock"));
		return false;
	}

	// AudioComponent 생성
	AudioComponent = UGameplayStatics::CreateSound2D(
		GetWorld(), MusicSound, MusicVolume, 1.f, 0.f, nullptr, false, false);
	if (!IsValid(AudioComponent))
	{
		RA_LOG(LogRefrain, Error, TEXT("Failed to create audio component"));
		return false;
	}
	
	// 재생
	FQuartzQuantizationBoundary QuantizationBoundary(EQuartzCommandQuantization::Bar);
	UQuartzClockHandle* RawClockHandle = MusicClockHandle.Get();
	AudioComponent->PlayQuantized(
		GetWorld(),
		RawClockHandle,
		QuantizationBoundary,
		FOnQuartzCommandEventBP(),
		MusicData->StartOffset);
	
	return IsValid(AudioComponent);
}

bool UMagicalTimingSubsystem::StopMusic()
{
	if (IsValid(AudioComponent))
	{
		AudioComponent->Stop();
		AudioComponent->DestroyComponent();
		AudioComponent = nullptr;
	}
	
	if (IsValid(MusicClockHandle))
	{
		UQuartzClockHandle* RawClockHandle = MusicClockHandle.Get();
		MusicClockHandle->StopClock(GetWorld(), true, RawClockHandle);
		MusicClockHandle = nullptr;
	}

	return true;
}

bool UMagicalTimingSubsystem::CreateQuartzClock()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	UQuartzSubsystem* QuartzSubsystem = UQuartzSubsystem::Get(World);
	if (!QuartzSubsystem || !QuartzSubsystem->IsQuartzEnabled())
	{
		RA_LOG(LogRefrain, Error, TEXT("Quartz subsystem is not available or disabled."));
		return false;
	}

	if (QuartzSubsystem->DoesClockExist(this, ClockName))
	{
		QuartzSubsystem->DeleteClockByName(this, ClockName);
	}

	// MusicData 설정 필요
	MusicClockHandle = QuartzSubsystem->CreateNewClock(
		this, ClockName, MusicData->ToQuartzClockSettings(), true);
	if (!MusicClockHandle)
	{
		RA_LOG(LogRefrain, Error, TEXT("Failed to create Quartz clock: %s"), *ClockName.ToString());
		return false;
	}
	UQuartzClockHandle* RawClockHandle = MusicClockHandle.Get();
	MusicClockHandle->SetBeatsPerMinute(
		World, FQuartzQuantizationBoundary(), FOnQuartzCommandEventBP(), RawClockHandle, MusicData->BPM);
	
	MusicClockHandle->StartClock(World, RawClockHandle);

	return true;
}
