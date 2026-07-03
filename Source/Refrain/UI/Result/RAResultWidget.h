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
	
	/** @brief Miss 누적 판정 수 표시 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAResultRowWidget> MissCountRow;
	
	/** @brief MaxHits 표시 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAResultRowWidget> MaxHitsRow;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 점수 UI 위젯 바인딩 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
};
