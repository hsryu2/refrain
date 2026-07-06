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
	UE_LOG(LogTemp, Warning, TEXT("GameplayCue Attack MyTarget: %s / Class: %s"),
	*GetNameSafe(MyTarget),
	MyTarget ? *MyTarget->GetClass()->GetName() : TEXT("None"));
	
	if (!MyTarget)
	{
		return false;
	}
	
	AActor* EffectTarget = MyTarget;
	
	if (!Cast<ACharacter>(EffectTarget))
	{
		EffectTarget = const_cast<AActor*>(Parameters.GetInstigator());
	}

	if (!EffectTarget)
	{
		return false;
	}
	
	if (AttackEffect)
	{
		if (const ACharacter* Character = Cast<ACharacter>(EffectTarget))
		{
			USkeletalMeshComponent* Mesh = Character->GetMesh();

			if (!Mesh)
			{
				return false;
			}
			
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
	
	//FVector SpawnLocation = Parameters.Location;
	//FRotator SpawnRotator = MyTarget->GetActorRotation();
	//
	//if (AttackEffect)
	//{
	//	if (const ACharacter* Character = Cast<ACharacter>(MyTarget))
	//	{
	//		USkeletalMeshComponent* Mesh = Character->GetMesh();
	//		
	//		UNiagaraFunctionLibrary::SpawnSystemAttached(
	//			AttackEffect,
	//			Mesh,
	//			AttachSocketName,
	//			FVector::ZeroVector,
	//			FRotator::ZeroRotator,
	//			EAttachLocation::SnapToTarget,
	//			true
	//		);
	//	}
	//}
	
	return true;
}
