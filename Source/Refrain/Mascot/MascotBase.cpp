// Fill out your copyright notice in the Description page of Project Settings.


#include "MascotBase.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
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
}

void AMascotBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateFollowTarget(DeltaTime);
	UpdateBeatSyncedNiagara();
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

void AMascotBase::UpdateBeatSyncedNiagara()
{
	if (!bSyncNiagaraToMusicBeat)
	{
		RA_LOG(LogRefrain, Log, TEXT("SyncNiagaraToMusicBeat Disabled"));
		return;
	}

	if (!IsValid(BeatSyncedNiagaraComponent))
	{
		BeatSyncedNiagaraComponent = FindComponentByClass<UNiagaraComponent>();
		if (!IsValid(BeatSyncedNiagaraComponent))
		{
			RA_LOG(LogRefrain, Error, TEXT("BeatSyncedNiagaraComponent Not Found"));
			return;
		}
	}

	if (!IsValid(CachedTimingSubsystem))
	{
		if (UWorld* World = GetWorld())
		{
			CachedTimingSubsystem = World->GetSubsystem<UMagicalTimingSubsystem>();
		}
	}

	if (!IsValid(CachedTimingSubsystem) || !CachedTimingSubsystem->IsMusicPlaying())
	{
		RA_LOG(LogRefrain, Warning, TEXT("Music Not Playing"));
		BeatSyncedNiagaraComponent->Deactivate();
		LastNiagaraBeatBar = INDEX_NONE;
		LastNiagaraBeat = INDEX_NONE;
		return;
	}

	FQuartzTransportTimeStamp CurrentTimeStamp;
	if (!CachedTimingSubsystem->GetMusicTimeStamp(CurrentTimeStamp))
	{
		RA_LOG(LogRefrain, Error, TEXT("Failed to get current timestamp"));
		return;
	}

	const float SecondsPerBeat = CachedTimingSubsystem->GetSecondsPerBeat();
	if (SecondsPerBeat <= SMALL_NUMBER)
	{
		RA_LOG(LogRefrain, Error, TEXT("SecondsPerBeat <= SMALL_NUMBER"));
		return;
	}

	const float TargetPlayRate = FMath::Clamp(
		BeatSyncedNiagaraBaseDuration / SecondsPerBeat,
		MinNiagaraPlayRate,
		MaxNiagaraPlayRate);
	BeatSyncedNiagaraComponent->SetCustomTimeDilation(TargetPlayRate);

	if (LastNiagaraBeatBar == INDEX_NONE || LastNiagaraBeat == INDEX_NONE)
	{
		LastNiagaraBeatBar = CurrentTimeStamp.Bars;
		LastNiagaraBeat = CurrentTimeStamp.Beat;
		return;
	}

	if (CurrentTimeStamp.Bars == LastNiagaraBeatBar && CurrentTimeStamp.Beat == LastNiagaraBeat)
	{
		return;
	}

	LastNiagaraBeatBar = CurrentTimeStamp.Bars;
	LastNiagaraBeat = CurrentTimeStamp.Beat;
	BeatSyncedNiagaraComponent->Activate(true);
}
