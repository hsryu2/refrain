// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MascotBase.generated.h"

UCLASS()
class REFRAIN_API AMascotBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMascotBase();
	
protected:
// 재정의 함수
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// 따라다닐 캐릭터 설정
	FORCEINLINE void SetFollowTarget(AActor* Target) { FollowTarget = Target; }
	
protected:
	// 위치 갱신 - Tick마다 실행되는 함수
	void UpdateFollowTarget(float DeltaTime);
	
protected:
	// 마스코트가 따라다닐 대상
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mascot")
	TObjectPtr<AActor> FollowTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mascot")
	FVector FollowOffset = FVector(-80.f, 60.f, 100.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mascot")
	float FollowInterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mascot")
	float HoverAmplitude = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mascot")
	float HoverSpeed = 2.f;

	float HoverTime = 0.f;
	
};
