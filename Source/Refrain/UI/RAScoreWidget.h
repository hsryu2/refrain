// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/RAPlayerState.h"
#include "RAScoreWidget.generated.h"

class UTextBlock;

/**
 * @brief 전투 중 점수, 콤보, 판정 결과를 실시간으로 갱신하여 보여주는 위젯 클래스
 */
UCLASS()
class REFRAIN_API URAScoreWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	/**
	 * @brief 점수 변경 이벤트를 처리하고 UI 텍스트를 갱신합니다.
	 * @param Judgement 이번 타격의 판정 결과
	 * @param AddedScore 이번 타격으로 추가된 점수
	 * @param NewTotalScore 갱신된 총 점수
	 * @param NewCombo 갱신된 현재 콤보 수
	 */
	UFUNCTION()
	void HandleScoreUpdated(ERAHitJudgement Judgement, int32 AddedScore, int32 NewTotalScore, int32 NewCombo);

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 점수 UI 텍스트 바인딩 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 현재까지 누적된 총 점수를 표시 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalScoreText;

	/** @brief 현재 달성한 콤보 수를 표시 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ComboText;

	/** @brief 최근 타격 판정 (Perfect, Good 등)을 표시 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> JudgementText;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 점수 UI 텍스트 바인딩 --- @/
	// ----------------------------------------------------------------------------------------------------------------
};
