// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackHitSweepComponent.generated.h"


class ARACharacterBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class REFRAIN_API UAttackHitSweepComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttackHitSweepComponent();
	
public:
	UFUNCTION(BlueprintCallable, Category = Targeting)
	TArray<AActor*> HitSweep() const;
	
	// 타겟 클래스 설정 (플레이어 -> NPC or NPC -> 플레이어)
	UFUNCTION(BlueprintCallable, Category = Targeting)
	void SetTargetActorClass(TSubclassOf<AActor> InTargetActorClass);
	
// 판정 범위 변수
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	float SweepDistance = 75.0f; 

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	float SweepStartOffset  = 75.0f; 
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	float SphereSize  = 75.0f;
	
// 판정 대상 클래스 변수
protected:
	// 타겟팅할 콜리전채널을 확인.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	TEnumAsByte<ECollisionChannel> TargetCollisionChannel = ECC_Pawn;
	
	// 타겟의 클래스를 한정하기 위해 사용.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	TSubclassOf<ARACharacterBase> TargetActorClass;
	
private:
	bool IsValidTarget(AActor* TargetActor) const;
};
