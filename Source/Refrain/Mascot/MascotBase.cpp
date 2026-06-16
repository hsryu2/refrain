// Fill out your copyright notice in the Description page of Project Settings.


#include "MascotBase.h"

#include "Kismet/GameplayStatics.h"

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
}

void AMascotBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
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
