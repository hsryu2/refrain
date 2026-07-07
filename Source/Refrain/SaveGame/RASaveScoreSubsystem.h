// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RASaveScoreSubsystem.generated.h"

/**
 * 
 */
class URASaveScore;
 
UCLASS()
class REFRAIN_API URASaveScoreSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// 서브시스템 초기화
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 서브시스템 해제 
	virtual void Deinitialize() override;
	
	// 곡의 최고 점수 가져오기.
	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetSongHighScore(FName SongID) const;
	
	// 최고 점수 갱신 및 세이브 -> 곡 종료시 호출할 예정.
	UFUNCTION(BlueprintCallable, Category = "Score")
	void UpdateAndSaveSongHighScore(FName SongID, int32 CurrentScore);
	
private:
	UPROPERTY()
	URASaveScore* SaveScoreInstance;
	
	const FString SaveSlotName = TEXT("RAScoreSlot");
};
