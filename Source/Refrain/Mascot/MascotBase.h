// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MascotBase.generated.h"

class UMagicalTimingSubsystem;
class UNiagaraComponent;

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

	// 비트 표시 (나이아가라 갱신)
	void InitializeBeatSyncedNiagara();
	void UpdateBeatSyncedNiagara();
	
// 마스코트가 따라다닐 대상
protected:
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

// 나이아가라 관련 변수
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mascot|VFX")
	bool bSyncNiagaraToMusicBeat = true;

	// 나이아가라 효과 크기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mascot|VFX", meta=(ClampMin="0.01"))
	float BeatSyncedNiagaraScale = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mascot|VFX", meta=(ClampMin="0.01"))
	float BeatSyncedNiagaraBaseDuration = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mascot|VFX", meta=(ClampMin="0.1", ClampMax="10.0"))
	float MinNiagaraPlayRate = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mascot|VFX", meta=(ClampMin="0.1", ClampMax="10.0"))
	float MaxNiagaraPlayRate = 10.f;

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> BeatSyncedNiagaraComponent;

	UPROPERTY(Transient)
	TObjectPtr<UMagicalTimingSubsystem> CachedTimingSubsystem;

	int32 LastNiagaraBeatBar = INDEX_NONE;
	int32 LastNiagaraBeat = INDEX_NONE;
	
};
