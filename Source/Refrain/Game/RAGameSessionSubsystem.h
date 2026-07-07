// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RAGameSessionSubsystem.generated.h"

class UMagicalMusicData;

/**
 * @brief 게임 실행 내내 전역적으로 유지되는 세션 데이터 서브시스템
 * @note 곡 선택 화면에서 고른 데이터를 인게임 맵으로 넘길 때 사용합니다.
 */
UCLASS()
class REFRAIN_API URAGameSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	/** @brief 현재 유저가 선택해서 플레이할 곡의 데이터 */
	UPROPERTY(BlueprintReadWrite, Category = "Session")
	TObjectPtr<UMagicalMusicData> SelectedSong;

	/** @brief 서브시스템 초기화 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** @brief 서브시스템 종료 */
	virtual void Deinitialize() override;
};
