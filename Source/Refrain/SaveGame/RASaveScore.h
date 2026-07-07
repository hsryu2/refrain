// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RASaveScore.generated.h"

/**
 * 
 */
UCLASS()
class REFRAIN_API URASaveScore : public USaveGame
{
	GENERATED_BODY()
	
public:
	URASaveScore();
	
	// 최고 점수를 저장할 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	int32 HighScore;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveData)
	TMap<FName, int32> SongHighScores;
	
};
