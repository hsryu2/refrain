// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RAMainMenuWidget.generated.h"

/** 
 * @enum EMainMenuSwitch 
 * @brief 메뉴를 스위칭할 열거형
 */
enum class EMainMenuSwitch : uint8
{
	MainMenu = 0,
	Settings = 1
};

class URAMenuButtonWidget;

/**
 * @brief 프로젝트의 메인 메뉴와 설정 화면을 제어하는 위젯 클래스\n
 * @note 메인메뉴의 기능을 담당하며 위젯 블루프린트 작성시 바인딩하여 작성하도록 합니다.\n
 * @date 2026-06-29\n
 * @author sejong
 */
UCLASS()
class REFRAIN_API URAMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** 
	 * @brief 선택 효과 배경 위젯 
	 * @note 메인 메뉴 전용
	 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> SelectionHighlight;

	/** 
	 * @brief 버튼 사이의 Y축 간격 
	 * @note 에디터에서 UI 간격에 맞게 조절합니다.\n 메인 메뉴 전용 
	 */
	UPROPERTY(EditDefaultsOnly, Category = "UI|Animation")
	float MenuSpacing = 80.0f;

	/** @brief 목표 Y 좌표 */
	float TargetTranslationY = 0.0f;
	
	/** @brief 현재 Y 좌표 */
	float CurrentTranslationY = 0.0f;

	/** @brief 현재 선택된(Active) 메뉴 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	int32 SelectedIndex = 0;

	/** @brief 메인 메뉴와 설정 화면 전환을 위한 최상위 스위치 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> MenuSwitcher;

	// --- 메인 메뉴 UI 바인딩 ---
	
	/** @brief 게임 시작 버튼(이어하기) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAMenuButtonWidget> Btn_Continue;
	
	/** @brief 게임 시작 버튼(처음부터) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAMenuButtonWidget> Btn_NewGame;
	
	/** @brief 설정 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAMenuButtonWidget> Btn_Settings;
	
	/** @brief 크레딧 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAMenuButtonWidget> Btn_Credits;
	
	/** @brief 게임 종료 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAMenuButtonWidget> Btn_ExitGame;
	
	// /@ --- 설정 메뉴 UI 바인딩 ---
	
	// --- 버튼 호버 이벤트 (애니메이션용) ---

	/** 
	 * @brief SelectionHighlight 위치 업데이트 
	 * @details 마우스 커서가 버튼 영역을 벗어날 경우를 처리하기 위한 함수입니다. 단순히 Hover 처리를 한다면 OnHovered를 사용하면 되겠으나
	 * 시각적 효과를 위해 OnHovered 같은 기능을 별도로 사용하지 않습니다. 즉, 버튼 애니메이션과 버튼(글꼴)의 일관성을 위한 기능이라고 보면 됩니다.
	 * 예) 사다리꼴 이펙트는 마우스가 가리켰던 버튼의 이전 위치에 남지만 마우스가 벗어난 경우 hover 상태가 아니게 되어 어색함이 남음.
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void UpdateHighlightPosition(int32 MenuIndex);

	// --- 설정 메뉴 UI 바인딩 @/
	
	// /@ --- 버튼 클릭 이벤트 ---
	
	/** 
	 * @brief NewGame에서 이동할 맵 이름 
	 * @note UE에서 지정하세요.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TSoftObjectPtr<UWorld> NewGameLevel;
	
	/**
	 * @brief 게임 시작 버튼(처음부터) 클릭 이벤트
	 * @details 에디터에서 이동할 맵을 지정합니다.
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnNewGameClicked(int32 MenuIndex);
	
	/** 
	 * @brief 게임 종료 버튼 클릭 이벤트
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnExitGameClicked(int32 MenuIndex);
};
