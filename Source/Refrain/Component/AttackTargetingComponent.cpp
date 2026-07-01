// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTargetingComponent.h"
#include "Engine/OverlapResult.h"
#include "Refrain/Character/RACharacterNonPlayer.h"

#include "DrawDebugHelpers.h"

UAttackTargetingComponent::UAttackTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 타겟 NPC로 설정.
	TargetActorClass = ARACharacterNonPlayer::StaticClass();
}

AActor* UAttackTargetingComponent::FindAttackTarget() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}
	
	FVector OwnerLocation = Owner->GetActorLocation();
	FVector OwnerForwardVector = Owner->GetActorForwardVector();
	
	TArray<FOverlapResult> OverlapResults;
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(SearchRadius);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	
	bool bHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		OwnerLocation,
		FQuat::Identity,
		TargetCollisionChannel,
		Sphere,
		QueryParams
	);
	if (!bHit)
	{
		return nullptr;
	}
	
	// 감지된 타겟중 가장 적절한 타겟을 점수별로 설정.
	// FLT_MAX -> 실수값 최대치
	AActor* BestTarget = nullptr;
	float BestScore = -FLT_MAX;
	
	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* TargetActor = Result.GetActor();
		
		// 공격 대상이 맞는지 확인.
		if (!IsValidTarget(TargetActor))
		{
			continue;
		}
		
		// 공격대상을 타겟으로써 적절한지 점수를 매겨 설정.
		float Score = CalculateTargetScore(TargetActor);
		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = TargetActor;
		}
	}
	return BestTarget;
}

void UAttackTargetingComponent::SetTargetActorClass(TSubclassOf<AActor> InTargetActorClass)
{
	TargetActorClass = InTargetActorClass;
}

bool UAttackTargetingComponent::IsValidTarget(AActor* TargetActor) const
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
	FVector ToTarget = TargetActor->GetActorLocation() - Owner->GetActorLocation();
	ToTarget.Z = 0.0f;
	
	if (ToTarget.IsNearlyZero())
	{
		return false;
	}
	
	ToTarget.Normalize();
	
	FVector OwnerForward = Owner->GetActorForwardVector();
	OwnerForward.Z = 0.0f;
	OwnerForward.Normalize();
	
	float Dot = FVector::DotProduct(OwnerForward, ToTarget);
	
	float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
	
	// 현재 MaxTargetAngle을 120으로 맞춰놨으므로, 좌우 60도씩 확인.
	if (Angle > MaxTargetAngle * 0.5f)
	{
		return false;
	}
	return true;
}

float UAttackTargetingComponent::CalculateTargetScore(AActor* TargetActor) const
{
	AActor* Owner = GetOwner();
	
	// null 체크
	// 점수를 반환해야하므로 없으니까 실수 최소값 반환.
	if (!Owner || !TargetActor)
	{
		return -FLT_MAX;
	}
	
	// 감지된 대상과 플레이어간의 거리를 구함.
	FVector ToTarget = TargetActor->GetActorLocation() - Owner->GetActorLocation();
	
	float Distance = ToTarget.Size();
	
	ToTarget.Z = 0.0f;
	ToTarget.Normalize();
	
	FVector OwnerForward = Owner->GetActorForwardVector();
	OwnerForward.Z = 0.0f;
	OwnerForward.Normalize();
	
	float ForwardDot = FVector::DotProduct(OwnerForward, ToTarget);
	
	//  거리 점수와 전방에 있는 적에 대한 점수
	float DistanceScore = 1.0f - FMath::Clamp(Distance / SearchRadius, 0.0f, 1.0f);
	float ForwardScore = FMath::Clamp(ForwardDot, 0.0f, 1.0f);
	
	// 전방에 있는 적이면 점수를 더 줌.
	return ForwardScore * 3.0f + DistanceScore; 
}




