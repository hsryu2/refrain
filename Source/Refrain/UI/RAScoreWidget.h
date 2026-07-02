// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/RAPlayerState.h"
#include "RAScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class REFRAIN_API URAScoreWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void HandleScoreUpdated(ERAHitJudgement Judgement, int32 AddedScore, int32 NewTotalScore, int32 NewCombo);
};
