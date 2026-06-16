// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGC_Attack.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

URAGC_Attack::URAGC_Attack()
{
	GameplayCueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Attack"));
}

bool URAGC_Attack::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (!MyTarget)
	{
		return false;
	}
	
	FVector SpawnLocation = Parameters.Location;
	FRotator SpawnRotator = MyTarget->GetActorRotation();
	
	if (AttackEffect)
	{
		if (const ACharacter* Character = Cast<ACharacter>(MyTarget))
		{
			USkeletalMeshComponent* Mesh = Character->GetMesh();
			
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				AttackEffect,
				Mesh,
				AttachSocketName,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				true
			);
		}
	}
	
	return true;
}
