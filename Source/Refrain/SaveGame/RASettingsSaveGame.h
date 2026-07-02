// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RASettingsSaveGame.generated.h"

/**
 * @brief 오디오 볼륨 등 환경설정을 디스크에 저장하기 위한 클래스
 * @date 2026-07-01
 * @author Sejong
 */
UCLASS()
class REFRAIN_API URASettingsSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	URASettingsSaveGame();

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 오디오 설정 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 마스터 볼륨 저장 값 (0.0 ~ 1.0) */
	UPROPERTY(VisibleAnywhere, Category = "Settings|Audio")
	float MasterVolume;

	/** @brief BGM 볼륨 저장 값 (0.0 ~ 1.0) */
	UPROPERTY(VisibleAnywhere, Category = "Settings|Audio")
	float BGMVolume;

	/** @brief SFX 볼륨 저장 값 (0.0 ~ 1.0) */
	UPROPERTY(VisibleAnywhere, Category = "Settings|Audio")
	float SFXVolume;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 오디오 설정 --- @/
	// ----------------------------------------------------------------------------------------------------------------
};
