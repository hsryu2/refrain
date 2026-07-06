// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/RAGameSessionSubsystem.h"
#include "Timing/MagicalMusicData.h"

void URAGameSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SelectedSong = nullptr;
}

void URAGameSessionSubsystem::Deinitialize()
{
	SelectedSong = nullptr;
	Super::Deinitialize();
}
