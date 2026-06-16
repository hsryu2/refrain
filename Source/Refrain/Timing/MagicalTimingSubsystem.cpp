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

float UMagicalTimingSubsystem::GetBeatProgress()
{
	if (!IsValid(MusicClockHandle))
	{
		return 0.f;
	}
	return MusicClockHandle->GetBeatProgressPercent();
}

float UMagicalTimingSubsystem::JudgeTiming(EQuartzCommandQuantization TargetQuantization, float Multiplier)
{
	FQuartzTransportTimeStamp TimeStamp = MusicClockHandle->GetCurrentTimestamp(GetWorld());
	if (TimeStamp.IsZero())
	{
		RA_LOG(LogRefrain, Error, TEXT("Failed to get current timestamp"));
		return 0.f;
	}
	RA_LOG(LogRefrain, Log, TEXT("Current TimeStamp Bar: %d, Beat: %d, Fraction: %f, Seconds: %f"), TimeStamp.Bars, TimeStamp.Beat, TimeStamp.BeatFraction, TimeStamp.Seconds);
	
	const float TargetDuration = MusicClockHandle->GetDurationOfQuantizationTypeInSeconds(GetWorld(), TargetQuantization, Multiplier);
	// TODO: Multiplier 계산 구현
	float TargetProgress = MusicClockHandle->GetBeatProgressPercent(TargetQuantization);
	
	const float TimeSincePreviousTarget = TargetDuration * TargetProgress;
	const float TimeUntilNextTarget = TargetDuration * (1.f - TargetProgress);
	
	// 이전 박부터 시간과 다음 박까지 시간 중 더 가까운 값
	const float SignedOffsetFromNearestBeat = 
		TargetProgress <= 0.5f ? TimeSincePreviousTarget: -(TimeUntilNextTarget);

	return SignedOffsetFromNearestBeat;
}

float UMagicalTimingSubsystem::GetTimeUntilNextHit(float MinimumStartupDelay, EQuartzCommandQuantization TargetQuantization, float Multiplier)
{
	const float TargetDuration = MusicClockHandle->GetDurationOfQuantizationTypeInSeconds(GetWorld(), TargetQuantization, Multiplier);
	float TargetProgress = MusicClockHandle->GetBeatProgressPercent(TargetQuantization);
	
	float TimeUntilNextHit = TargetDuration * (1.f - TargetProgress);
	
	// 최소 선딜레이 적용
	while (TimeUntilNextHit < MinimumStartupDelay)
	{
		TimeUntilNextHit += TargetDuration;
	}
	
	return TimeUntilNextHit;
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
