// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MagicalTimingSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class REFRAIN_API UMagicalTimingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UMagicalTimingSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Music, meta=(ClampMin=60, ClampMax=240))
	float MusicBPM;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Music)
	FName ClockName = TEXT("MusicClock");
};
