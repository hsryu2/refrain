// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RASongItemData.generated.h"

/**
 * @brief UListView에서 사용할 곡 데이터 오브젝트
 */
UCLASS(BlueprintType)
class REFRAIN_API URASongItemData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Info")
	FString SongTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Info")
	FString Artist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Info")
	float BPM = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Info")
	TObjectPtr<class UTexture2D> JacketImage;
};
