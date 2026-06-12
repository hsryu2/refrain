// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicalTimingSubsystem.h"

#include "Refrain/Refrain.h"

UMagicalTimingSubsystem::UMagicalTimingSubsystem()
{
}

void UMagicalTimingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RA_LOG(LogRefrain, Log, TEXT("MagicalTimingSubsystem Initialize"));
}

void UMagicalTimingSubsystem::Deinitialize()
{
	Super::Deinitialize();
	RA_LOG(LogRefrain, Log, TEXT("MagicalTimingSubsystem Deinitialize"));
}
