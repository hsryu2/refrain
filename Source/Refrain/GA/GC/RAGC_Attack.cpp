// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGC_Attack.h"
#include "NiagaraFunctionLibrary.h"
#include "Refrain.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

URAGC_Attack::URAGC_Attack()
{
	
}

bool URAGC_Attack::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (!AttackEffect)
	{
		RA_LOG(LogRefrain, Warning, TEXT("AttackEffect Not Found"));
		return false;
	}

	const FName* SocketName = AttackSocketMap.Find(Parameters.OriginalTag);
	if (!SocketName)
	{
		RA_LOG(LogRefrain, Warning, TEXT("SocketName Not Found"));
		return false;
	}
	
	if (!MyTarget)
	{
		RA_LOG(LogRefrain, Warning, TEXT("MyTarget Not Found"));
		return false;
	}
	
	AActor* EffectTarget = MyTarget;
	
	if (!Cast<ACharacter>(EffectTarget))
	{
		EffectTarget = const_cast<AActor*>(Parameters.GetInstigator());
	}

	if (!EffectTarget)
	{
		RA_LOG(LogRefrain, Warning, TEXT("EffectTarget Not Found"));
		return false;
	}
	
	if (AttackEffect)
	{
		if (const ACharacter* Character = Cast<ACharacter>(EffectTarget))
		{
			USkeletalMeshComponent* Mesh = Character->GetMesh();

			if (!Mesh)
			{
				RA_LOG(LogRefrain, Warning, TEXT("Mesh Not Found"));
				return false;
			}
			
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				AttackEffect,
				Mesh,
				*SocketName,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				true
			);
		}
	}
	
	
	return true;
}
