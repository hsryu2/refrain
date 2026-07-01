// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackHitSweepComponent.h"

#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "DrawDebugHelpers.h"
#include "Character/RACharacterBase.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UAttackHitSweepComponent::UAttackHitSweepComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}


TArray<AActor*> UAttackHitSweepComponent::HitSweep() const
{
	TArray<AActor*> HitActors;
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return HitActors;
	}
	
	FVector OwnerLocation = Owner->GetActorLocation();
	FVector ForwardVector = Owner->GetActorForwardVector();
	
	// 히트스윕 시작 위치와 끝 위치
	FVector StartLocation = OwnerLocation + (ForwardVector * SweepStartOffset);
	FVector EndLocation = StartLocation + (ForwardVector * SweepDistance);
	
	TArray<FHitResult> HitResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereSize); 
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		TargetCollisionChannel,
		Sphere,
		QueryParams
	);
	
	// 히트스윕 확인용
	FColor DrawColor = bHit ? FColor::Green : FColor::Red;
	
	DrawDebugSphere(GetWorld(), StartLocation, SphereSize, 12, DrawColor, false, 2.0f);
	DrawDebugSphere(GetWorld(), EndLocation, SphereSize, 12, DrawColor, false, 2.0f);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, DrawColor, false, 2.0f, 0, 2.0f);
	// 여기까지 히트스윕 확인용
	
	if (bHit)
	{
		for (const FHitResult& Result : HitResults)
		{
			AActor* TargetActor = Result.GetActor();
			if (IsValidTarget(TargetActor) && !HitActors.Contains(TargetActor))
			{
				HitActors.Add(TargetActor);
			}
		}
	}
	return HitActors;
}

void UAttackHitSweepComponent::SetTargetActorClass(TSubclassOf<AActor> InTargetActorClass)
{
	TargetActorClass = InTargetActorClass;
}

bool UAttackHitSweepComponent::IsValidTarget(AActor* TargetActor) const
{
	AActor* Owner = GetOwner();
	 
	// null 체크
	if (!Owner || !TargetActor)
	{
		return false;
	}
	// 자기 자신 제외
	if (TargetActor == Owner)
	{
		return false;
	}
	// 검사한 액터가 타겟이 될 수 있는 액터인지 클래스로 확인
	if (TargetActorClass && !TargetActor->IsA(TargetActorClass))
	{
		return false;
	}
	return true;
}
