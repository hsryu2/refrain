// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RACharacterSpawner.generated.h"

/**
 * @brief NPC(RACharacterNonPlayer)를 레벨에 소환하고 관리하는 스포너 액터 클래스입니다.
 * @author Sejong
 * @date 2026-06-22
 */
UCLASS()
class REFRAIN_API ARACharacterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARACharacterSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** @brief 스폰할 NPC의 클래스 정보 (블루프린트에서 할당합니다) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<class ARACharacterNonPlayer> CharacterClassToSpawn;

	/** @brief 소환진(Summon Circle) 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<class AActor> SummonCircleClass;

	/** @brief 소환진 생성 후 실제 몬스터가 나오는 딜레이 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SummonDelay = 2.0f;

	/** @brief 최대로 생성할 NPC의 수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 MaxSpawnCount = 10;
	
	/** @brief 몬스터를 생성할 주기 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnInterval = 5.0f;
	
	// --- 스폰 범위 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Area")
	float MinX = -350.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Area")
	float MaxX = 260.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Area")
	float MinY = -350.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Area")
	float MaxY = 260.0f;
	
private:
	/** @brief Monster Array 테이블 */
	UPROPERTY()
	TArray<AActor*> SpawnedMonsters;
	
	FTimerHandle SpawnTimerHandle;
	
	// --- 내부 함수 ---
	void CheckAndSpawn();
	void SpawnMonsters(int32 SpawnCount);
	void SpawnMonsterAtLocation(FVector SpawnLocation);
	int32 GetCurrentMonsterCount();
};
