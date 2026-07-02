// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicalTimingSubsystem.h"

#include "Components/AudioComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "Quartz/QuartzSubsystem.h"
#include "Refrain/Refrain.h"
#include "Refrain/Settings/RefrainClassSettings.h"
#include "Sound/SoundBase.h"

UMagicalTimingSubsystem::UMagicalTimingSubsystem()
{
}

bool UMagicalTimingSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// return Super::DoesSupportWorldType(WorldType);
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UMagicalTimingSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	const URefrainClassSettings* Settings = GetDefault<URefrainClassSettings>();
	
	// MagicalTimingMPCInstance 설정
	UMaterialParameterCollection* MagicalTimingMPC = Settings->MagicalTimingMPC.LoadSynchronous();
	if (!IsValid(MagicalTimingMPC))
	{
		RA_LOG(LogRefrain, Error, TEXT("MagicalTimingMPC is invalid"));
		return;
	}
	MagicalTimingMPCInstance = GetWorld()->GetParameterCollectionInstance(MagicalTimingMPC);
	if (!IsValid(MagicalTimingMPCInstance))
	{
		RA_LOG(LogRefrain, Error, TEXT("MagicalTimingMPCInstance is invalid"));
		return;
	}
}

void UMagicalTimingSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 비트 진행도를 UI 표시용 머티리얼에 전달
	if (IsValid(MagicalTimingMPCInstance))
	{
		MagicalTimingMPCInstance->SetScalarParameterValue(TEXT("BeatProgress"), GetBeatProgress());
	}
}

TStatId UMagicalTimingSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMagicalTimingSubsystem, STATGROUP_Tickables);
}

bool UMagicalTimingSubsystem::IsTickable() const
{
	return !IsTemplate() && IsValid(MusicClockHandle);
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

	// UAudioComponent 생성 및 포인터 저장
	MusicAudioComponent = UGameplayStatics::CreateSound2D(
		GetWorld(), MusicSound, MusicVolume, 1.f, 0.f, nullptr, false, false);
	if (!IsValid(MusicAudioComponent))
	{
		RA_LOG(LogRefrain, Error, TEXT("Failed to create audio component"));
		return false;
	}
	
	// 재생
	FQuartzQuantizationBoundary QuantizationBoundary(EQuartzCommandQuantization::Bar);
	UQuartzClockHandle* RawClockHandle = MusicClockHandle.Get();
	MusicAudioComponent->PlayQuantized(
		GetWorld(),
		RawClockHandle,
		QuantizationBoundary,
		FOnQuartzCommandEventBP(),
		MusicData->StartOffset);
	
	return IsValid(MusicAudioComponent);
}

bool UMagicalTimingSubsystem::StopMusic()
{
	if (IsValid(MusicAudioComponent))
	{
		MusicAudioComponent->Stop();
		MusicAudioComponent->DestroyComponent();
		MusicAudioComponent = nullptr;
	}
	
	if (IsValid(MusicClockHandle))
	{
		UQuartzClockHandle* RawClockHandle = MusicClockHandle.Get();
		MusicClockHandle->StopClock(GetWorld(), true, RawClockHandle);
		MusicClockHandle = nullptr;
	}

	return true;
}

void UMagicalTimingSubsystem::PauseMusic()
{
	if (IsValid(MusicAudioComponent))
	{
		MusicAudioComponent->SetPaused(true);
	}
	
	if (IsValid(MusicClockHandle))
	{
		UQuartzClockHandle* RawClockHandle = MusicClockHandle.Get();
		MusicClockHandle->PauseClock(GetWorld(), RawClockHandle);
	}
}

void UMagicalTimingSubsystem::ResumeMusic()
{
	if (IsValid(MusicAudioComponent))
	{
		MusicAudioComponent->SetPaused(false);
	}
	
	if (IsValid(MusicClockHandle))
	{
		UQuartzClockHandle* RawClockHandle = MusicClockHandle.Get();
		MusicClockHandle->ResumeClock(GetWorld(), RawClockHandle);
	}
}

bool UMagicalTimingSubsystem::PlaySFXQuantized(USoundBase* InSound, EQuartzCommandQuantization InQuantization, float InMultiplier)
{
	if (!IsMusicPlaying())
	{
		RA_LOG(LogRefrain, Error, TEXT("Music Not Playing"));
		return false;
	}
	if (!IsValid(InSound))
	{
		RA_LOG(LogRefrain, Error, TEXT("Sound is not valid"));
		return false;
	}
	
	// UAudioComponent 생성
	UAudioComponent* SFXAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), InSound);
	if (!IsValid(SFXAudioComponent))
	{
		RA_LOG(LogRefrain, Error, TEXT("Failed to create audio component"));
		return false;
	}
	
	// 재생
	FQuartzQuantizationBoundary QuantizationBoundary(
		InQuantization,
		FMath::Max(1.f, InMultiplier),
		EQuarztQuantizationReference::CurrentTimeRelative);
	UQuartzClockHandle* RawClockHandle = MusicClockHandle.Get();
	SFXAudioComponent->PlayQuantized(
		GetWorld(),
		RawClockHandle,
		QuantizationBoundary,
		FOnQuartzCommandEventBP());
	
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

bool UMagicalTimingSubsystem::IsMusicPlaying()
{
	return IsValid(MusicClockHandle) && IsValid(MusicAudioComponent) && MusicAudioComponent->IsPlaying();
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

float UMagicalTimingSubsystem::GetTimeUntilNextBeat(EQuartzCommandQuantization TargetQuantization, int MinBeatNum)
{
	const float TargetDuration = MusicClockHandle->GetDurationOfQuantizationTypeInSeconds(GetWorld(), TargetQuantization);
	float TargetProgress = MusicClockHandle->GetBeatProgressPercent(TargetQuantization);
	
	float TimeUntilNextHit = TargetDuration * (1.f - TargetProgress);
	
	/*// 최소 선딜레이 적용
	while (TimeUntilNextHit < MinimumStartupDelay)
	{
		TimeUntilNextHit += TargetDuration;
	}*/

	TimeUntilNextHit += (MinBeatNum - 1) * TargetDuration;
	
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
