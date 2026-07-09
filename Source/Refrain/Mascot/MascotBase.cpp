// Fill out your copyright notice in the Description page of Project Settings.


#include "MascotBase.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "OnlineSubsystemUtils.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "Refrain.h"
#include "Engine/World.h"
#include "Timing/MagicalTimingSubsystem.h"

// Sets default values
AMascotBase::AMascotBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMascotBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (!FollowTarget)
	{
		FollowTarget = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	InitializeBeatSyncedNiagara();
	BindMagicalTimingDelegates();
}

void AMascotBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnsubscribeFromBeatEvent();
	UnbindMagicalTimingDelegates();

	Super::EndPlay(EndPlayReason);
}

void AMascotBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 위치 갱신
	UpdateFollowTarget(DeltaTime);
}

void AMascotBase::UpdateFollowTarget(float DeltaTime)
{
	if (!IsValid(FollowTarget))
	{
		return;
	}
	
	HoverTime += DeltaTime;

	const FVector TargetLocation = FollowTarget->GetActorLocation();
	const FRotator TargetRotation = FollowTarget->GetActorRotation();

	const FVector DesiredLocation =
		TargetLocation
		+ TargetRotation.RotateVector(FollowOffset)
		+ FVector(0.f, 0.f, FMath::Sin(HoverTime * HoverSpeed) * HoverAmplitude);

	const FVector NewLocation = FMath::VInterpTo(
		GetActorLocation(),
		DesiredLocation,
		DeltaTime,
		FollowInterpSpeed
	);

	SetActorLocation(NewLocation);
}

void AMascotBase::InitializeBeatSyncedNiagara()
{
	if (!bSyncNiagaraToMusicBeat)
	{
		RA_LOG(LogRefrain, Log, TEXT("SyncNiagaraToMusicBeat Disabled"));
		return;
	}

	BeatSyncedNiagaraComponent = FindComponentByClass<UNiagaraComponent>();
	if (!IsValid(BeatSyncedNiagaraComponent))
	{
		RA_LOG(LogRefrain, Error, TEXT("BeatSyncedNiagaraComponent Not Found"));
		return;
	}

	const float ClampedScale = FMath::Max(BeatSyncedNiagaraScale, KINDA_SMALL_NUMBER);
	BeatSyncedNiagaraComponent->SetRelativeScale3D(FVector(ClampedScale));
	BeatSyncedNiagaraComponent->Deactivate();
}

void AMascotBase::BindMagicalTimingDelegates()
{
	if (!bSyncNiagaraToMusicBeat)
	{
		RA_LOG(LogRefrain, Log, TEXT("SyncNiagaraToMusicBeat Disabled"));
		return;
	}

	if (!IsValid(CachedTimingSubsystem))
	{
		if (UWorld* World = GetWorld())
		{
			CachedTimingSubsystem = World->GetSubsystem<UMagicalTimingSubsystem>();
		}
	}

	if (!IsValid(CachedTimingSubsystem))
	{
		RA_LOG(LogRefrain, Error, TEXT("MagicalTimingSubsystem Not Found"));
		return;
	}

	CachedTimingSubsystem->OnMusicStarted.AddUniqueDynamic(this, &AMascotBase::HandleMusicStarted);
	CachedTimingSubsystem->OnMusicFinished.AddUniqueDynamic(this, &AMascotBase::HandleMusicFinished);
	CachedTimingSubsystem->OnMusicPaused.AddUniqueDynamic(this, &AMascotBase::HandleMusicPaused);
	CachedTimingSubsystem->OnMusicResumed.AddUniqueDynamic(this, &AMascotBase::HandleMusicResumed);

	if (CachedTimingSubsystem->IsMusicPlaying())
	{
		SubscribeToBeatEvent();
	}
}

void AMascotBase::UnbindMagicalTimingDelegates()
{
	if (!IsValid(CachedTimingSubsystem))
	{
		return;
	}

	CachedTimingSubsystem->OnMusicStarted.RemoveDynamic(this, &AMascotBase::HandleMusicStarted);
	CachedTimingSubsystem->OnMusicFinished.RemoveDynamic(this, &AMascotBase::HandleMusicFinished);
	CachedTimingSubsystem->OnMusicPaused.RemoveDynamic(this, &AMascotBase::HandleMusicPaused);
	CachedTimingSubsystem->OnMusicResumed.RemoveDynamic(this, &AMascotBase::HandleMusicResumed);
}

void AMascotBase::RefreshBeatSyncedNiagaraForCurrentMusic()
{
	if (!IsValid(BeatSyncedNiagaraComponent))
	{
		BeatSyncedNiagaraComponent = FindComponentByClass<UNiagaraComponent>();
	}

	if (!IsValid(BeatSyncedNiagaraComponent) || !IsValid(CachedTimingSubsystem))
	{
		return;
	}

	CachedSecondsPerBeat = CachedTimingSubsystem->GetSecondsPerBeat();
	if (CachedSecondsPerBeat <= SMALL_NUMBER)
	{
		RA_LOG(LogRefrain, Error, TEXT("SecondsPerBeat <= SMALL_NUMBER"));
		return;
	}

	const float TargetPlayRate = FMath::Clamp(
		BeatSyncedNiagaraBaseDuration / CachedSecondsPerBeat,
		MinNiagaraPlayRate,
		MaxNiagaraPlayRate);
	BeatSyncedNiagaraComponent->SetCustomTimeDilation(TargetPlayRate);
}

void AMascotBase::SubscribeToBeatEvent()
{
	if (!bSyncNiagaraToMusicBeat)
	{
		return;
	}

	RefreshBeatSyncedNiagaraForCurrentMusic();

	if (!IsValid(BeatSyncedNiagaraComponent) || !IsValid(CachedTimingSubsystem) || !CachedTimingSubsystem->IsMusicPlaying())
	{
		return;
	}

	UQuartzClockHandle* MusicClockHandle = CachedTimingSubsystem->GetMusicClockHandle();
	if (!IsValid(MusicClockHandle))
	{
		RA_LOG(LogRefrain, Error, TEXT("MusicClockHandle Not Found"));
		return;
	}

	if (!bIsSubscribedToBeatEvent)
	{
		FOnQuartzMetronomeEventBP BeatDelegate;
		BeatDelegate.BindDynamic(this, &AMascotBase::HandleBeatEvent);
		MusicClockHandle->SubscribeToQuantizationEvent(
			this,
			EQuartzCommandQuantization::Beat,
			BeatDelegate,
			MusicClockHandle);
		bIsSubscribedToBeatEvent = true;
	}
}

void AMascotBase::UnsubscribeFromBeatEvent()
{
	if (bIsSubscribedToBeatEvent && IsValid(CachedTimingSubsystem))
	{
		UQuartzClockHandle* MusicClockHandle = CachedTimingSubsystem->GetMusicClockHandle();
		if (IsValid(MusicClockHandle))
		{
			MusicClockHandle->UnsubscribeFromTimeDivision(
				this,
				EQuartzCommandQuantization::Beat,
				MusicClockHandle);
		}
	}
	bIsSubscribedToBeatEvent = false;

	if (IsValid(BeatSyncedNiagaraComponent))
	{
		BeatSyncedNiagaraComponent->Deactivate();
	}
}

void AMascotBase::HandleMusicStarted()
{
	SubscribeToBeatEvent();
}

void AMascotBase::HandleMusicFinished()
{
	UnsubscribeFromBeatEvent();
}

void AMascotBase::HandleMusicPaused()
{
	UnsubscribeFromBeatEvent();
}

void AMascotBase::HandleMusicResumed()
{
	SubscribeToBeatEvent();
}

void AMascotBase::PlayBeatSyncedNiagara()
{
	if (!IsValid(BeatSyncedNiagaraComponent) || !IsValid(CachedTimingSubsystem) || !CachedTimingSubsystem->IsMusicPlaying())
	{
		UnsubscribeFromBeatEvent();
		return;
	}

	BeatSyncedNiagaraComponent->Activate(true);
}

void AMascotBase::HandleBeatEvent(FName ClockName, EQuartzCommandQuantization QuantizationType, int32 NumBars, int32 Beat, float BeatFraction)
{
	if (QuantizationType != EQuartzCommandQuantization::Beat)
	{
		return;
	}

	PlayBeatSyncedNiagara();
}
