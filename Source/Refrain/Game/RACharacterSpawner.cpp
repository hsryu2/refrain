// Fill out your copyright notice in the Description page of Project Settings.


#include "RACharacterSpawner.h"
#include "Character/RACharacterNonPlayer.h"

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

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < SpawnCount; ++i)
	{
		// 지정된 범위 내에서 랜덤 값 추출
		float RandomX = FMath::FRandRange(MinX, MaxX);
		float RandomY = FMath::FRandRange(MinY, MaxY);
		
		// Z축(위아래)은 0으로 두고 X, Y만 설정합니다.
		FVector SpawnLocation = FVector(RandomX, RandomY, 0.0f);

		// 스포너 액터의 현재 위치를 더해주면, 
		// 맵 어디에 스포너를 배치하든 그 주변을 기준으로 생성됩니다.
		SpawnLocation += GetActorLocation();

		AActor* NewMonster = World->SpawnActor<AActor>(CharacterClassToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        
		if (NewMonster)
		{
			SpawnedMonsters.Add(NewMonster);
		}
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

