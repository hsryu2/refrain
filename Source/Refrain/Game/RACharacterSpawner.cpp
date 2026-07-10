// Fill out your copyright notice in the Description page of Project Settings.


#include "RACharacterSpawner.h"

#include "TimerManager.h"
#include "Character/RACharacterNonPlayer.h"
#include "Engine/World.h"

// Sets default values
ARACharacterSpawner::ARACharacterSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARACharacterSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	// 타이머를 설정하여 SpawnInterval마다 CheckAndSpawn을 반복 호출
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ARACharacterSpawner::CheckAndSpawn, SpawnInterval, true);
}

// Called every frame
void ARACharacterSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARACharacterSpawner::CheckAndSpawn()
{
	int32 CurMonsterCnt = GetCurrentMonsterCount();
	int32 SpawnCount = MaxSpawnCount - CurMonsterCnt;

	if (SpawnCount > 0)
	{
		SpawnMonsters(SpawnCount);
	}
}

void ARACharacterSpawner::SpawnMonsters(int32 SpawnCount)
{
	UWorld* World = GetWorld();
	if (!World || !CharacterClassToSpawn) return;

	for (int32 i = 0; i < SpawnCount; ++i)
	{
		// 지정된 범위 내에서 랜덤 값 추출
		float RandomX = FMath::FRandRange(MinX, MaxX);
		float RandomY = FMath::FRandRange(MinY, MaxY);
		FVector SpawnLocation = FVector(RandomX, RandomY, 0.0f) + GetActorLocation();

		// 1. 소환진 클래스가 설정되어 있다면 먼저 스폰
		if (SummonCircleClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			World->SpawnActor<AActor>(SummonCircleClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			
			// 2. 타이머를 돌려서 SummonDelay 이후에 실제 몬스터를 해당 위치에 스폰
			FTimerHandle DelayTimerHandle;
			FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &ARACharacterSpawner::SpawnMonsterAtLocation, SpawnLocation);
			World->GetTimerManager().SetTimer(DelayTimerHandle, TimerDel, SummonDelay, false);
		}
		else
		{
			// 소환진이 안 설정되어 있으면 즉시 스폰
			SpawnMonsterAtLocation(SpawnLocation);
		}
	}
}

void ARACharacterSpawner::SpawnMonsterAtLocation(FVector SpawnLocation)
{
	UWorld* World = GetWorld();
	if (!World || !CharacterClassToSpawn) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* NewMonster = World->SpawnActor<AActor>(CharacterClassToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	
	if (NewMonster)
	{
		SpawnedMonsters.Add(NewMonster);
	}
}

int32 ARACharacterSpawner::GetCurrentMonsterCount()
{
	// 인덱스 꼬임을 방지하기 위한 삭제 처리
	for (int32 i = SpawnedMonsters.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(SpawnedMonsters[i]))
		{
			SpawnedMonsters.RemoveAt(i);
		}
	}
    
	return SpawnedMonsters.Num();
}

