// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RAResultWidget.generated.h"

class URAResultRowWidget;
class URAMenuButtonWidget;

/**
 * @brief 게임 결과 화면을 표시하는 위젯 클래스
 * @note 결과 화면에는 점수, 히트 수, 등등이 들어갈 수 있으며 게임 오버와 관련이 있을 수 있습니다. 이전 점수까지 고려할 수 있겠습니다.
 * @date 2026-07-03
 * @author Sejong 
 */
UCLASS()
class REFRAIN_API URAResultWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/** true = 게임 오버, false = 클리어 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Result")
	bool bIsGameOver = false;

	/** 점수 행을 일괄 숨기기/보이기 */
	UFUNCTION(BlueprintCallable, Category="Result")
	void SetScoreVisibility(bool bShow);

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 최고 기록 판별 ---
	// ----------------------------------------------------------------------------------------------------------------

	/** @brief 이번 판 점수가 기존 최고 점수를 넘었는지 여부 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Result")
	bool bIsNewRecord = false;

	/** @brief 이전 최고 점수 보관용 변수 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Result")
	int32 PreviousHighScore = 0;

	// ----------------------------------------------------------------------------------------------------------------
	// --- 최고 기록 판별 --- @/
	// ----------------------------------------------------------------------------------------------------------------

protected:
	virtual void NativeConstruct() override;
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 버튼 클릭 및 위젯 이벤트 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/**
	 * @brief 메인 메뉴로 돌아가기(Main Menu) 버튼 클릭 이벤트
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnMainMenuClicked(int32 MenuIndex);
	
	/**
	 * @brief 게임 재시작(Restart) 버튼 클릭 이벤트
	 * @note 레벨을 처음 불러왔을 때 처럼 합니다
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnRestartClicked(int32 MenuIndex);

	/**
	 * @brief 버튼 호버 시 선택 상태 업데이트
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void UpdateHighlightPosition(int32 MenuIndex);

	/** @brief 현재 선택된 버튼 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	int32 SelectedIndex = -1;

	// ----------------------------------------------------------------------------------------------------------------
	// --- 버튼 클릭 및 위젯 이벤트 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	/** 
	 * @brief MainMenuLevel에서 이동할 맵 이름 
	 * @note UE에서 지정하세요.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- UI 바인딩 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 메인 메뉴로 돌아가기 버튼 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<URAMenuButtonWidget> BtnMainMenu;

	/** @brief 메인 메뉴로 돌아가기 버튼 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<URAMenuButtonWidget> BtnRestart;

	
	// ----------------------------------------------------------------------------------------------------------------
	// --- UI 바인딩 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 점수 UI 위젯 바인딩 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 현재까지 누적된 총 점수를 표시(최종) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAResultRowWidget> TotalScoreRow;
	
	/** @brief Perfect 누적 판정 수 표시 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAResultRowWidget> PerfectCountRow;
	
	/** @brief Good 누적 판정 수 표시 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAResultRowWidget> GoodCountRow;
	
	/** @brief Bad 누적 판정 수 표시 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAResultRowWidget> BadCountRow;
	
	/** @brief MaxHits 표시 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URAResultRowWidget> MaxHitsRow;
	
	/** @brief 최고 점수(이전 기록) 표시 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URAResultRowWidget> HighScoreRow;

	// ----------------------------------------------------------------------------------------------------------------
	// --- 점수 UI 위젯 바인딩 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 패널 전환 (클리어 / 게임 오버) ---
	// ----------------------------------------------------------------------------------------------------------------

	/** @brief 화면 레이아웃을 전환하는 위젯 스위처 (0: 클리어, 1: 게임 오버) */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidgetSwitcher> ContentSwitcher;

	// ----------------------------------------------------------------------------------------------------------------
	// --- 패널 전환 (클리어 / 게임 오버) --- @/
	// ----------------------------------------------------------------------------------------------------------------

};
