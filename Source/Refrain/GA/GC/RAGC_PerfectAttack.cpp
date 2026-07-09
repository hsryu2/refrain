// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGC_PerfectAttack.h"

#include "NiagaraFunctionLibrary.h"
#include "Refrain.h"
#include "RefrainGameplayTags.h"
#include "Engine/World.h"

URAGC_PerfectAttack::URAGC_PerfectAttack()
{
}

bool URAGC_PerfectAttack::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (!PerfectAttackEffect)
	{
		RA_LOG(LogRefrain, Warning, TEXT("PerfectAttackEffect Not Found"));
		return false;
	}

	const AActor* TargetActor = Parameters.GetEffectCauser();
	if (!TargetActor)
	{
		TargetActor = MyTarget;
	}

	const FVector BaseLocation = TargetActor
		? TargetActor->GetActorLocation()
		: FVector(Parameters.Location);
	const FVector SpawnLocation = TargetActor && bOffsetInTargetLocalSpace
		? TargetActor->GetActorTransform().TransformPosition(TargetLocationOffset)
		: BaseLocation + TargetLocationOffset;

	FRotator SpawnRotation = FRotator::ZeroRotator;
	if (TargetActor && bUseTargetRotation)
	{
		SpawnRotation = TargetActor->GetActorRotation();
	}
	else if (!Parameters.Normal.IsNearlyZero())
	{
		SpawnRotation = Parameters.Normal.Rotation();
	}
	SpawnRotation += RotationOffset;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		PerfectAttackEffect,
		SpawnLocation,
		SpawnRotation,
		SpawnScale,
		true,
		true,
		ENCPoolMethod::AutoRelease,
		true
	);

	return true;
}
