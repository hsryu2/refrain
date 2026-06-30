// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RACharacterBase.h"
#include "Components/ActorComponent.h"
#include "AttackTargetingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REFRAIN_API UAttackTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackTargetingComponent();
	
	UFUNCTION(BlueprintCallable, Category = Targeting)
	AActor* FindAttackTarget() const;
	
	// 타겟 클래스 설정 (플레이어 -> NPC or NPC -> 플레이어)
	UFUNCTION(BlueprintCallable, Category = Targeting)
	void SetTargetActorClass(TSubclassOf<AActor> InTargetActorClass);
	
	UFUNCTION(BlueprintCallable, Category = Targeting)
	TArray<AActor*> HitSweep() const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	
	// 검색 반경
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	float SearchRadius = 250.0f;
	
	// 타겟찾는 반경 360도
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	float MaxTargetAngle = 360.0f;
	
	// 타겟팅할 콜리전채널을 확인.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	TEnumAsByte<ECollisionChannel> TargetCollisionChannel = ECC_Pawn;
	
	// 타겟의 클래스를 한정하기 위해 사용.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	TSubclassOf<ARACharacterBase> TargetActorClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	float SweepDistance = 75.0f; 

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	float SweepStartOffset  = 75.0f; 
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	float SphereSize  = 75.0f; 
	
private:
	bool IsValidTarget(AActor* TargetActor) const;
	float CalculateTargetScore(AActor* TargetActor) const;
		
};
