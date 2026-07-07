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
class URAVolumeSettingsMenuWidget;
class URASongSelectWidget;

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

public:
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 외부 설정 (에디터 노출) ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** 
	 * @brief 버튼 사이의 Y축 간격 
	 * @note 에디터에서 UI 간격에 맞게 조절합니다.\n 메인 메뉴 전용 
	 */
	UPROPERTY(EditDefaultsOnly, Category = "UI|Animation")
	float MenuSpacing = 80.0f;

	/** @brief 현재 선택된(Active) 메뉴 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	int32 SelectedIndex = 0;

	/** 
	 * @brief NewGame에서 이동할 맵 이름 
	 * @note UE에서 지정하세요.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TSoftObjectPtr<UWorld> NewGameLevel;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 외부 설정 (에디터 노출) --- @/
	// ----------------------------------------------------------------------------------------------------------------

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 내부 로직 변수 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 목표 Y 좌표 */
	float TargetTranslationY = 0.0f;
	
	/** @brief 현재 Y 좌표 */
	float CurrentTranslationY = 0.0f;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 내부 로직 변수 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 내부 로직 함수 (애니메이션) ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** 
	 * @brief SelectionHighlight 위치 업데이트 
	 * @details 마우스 커서가 버튼 영역을 벗어날 경우를 처리하기 위한 함수입니다.
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void UpdateHighlightPosition(int32 MenuIndex);
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 내부 로직 함수 (애니메이션) --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- UI 바인딩 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** 
	 * @brief 선택 효과 배경 위젯 
	 * @note 메인 메뉴 전용
	 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> SelectionHighlight;

	/** @brief 메인 메뉴와 설정 화면 전환을 위한 최상위 스위치 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> MenuSwitcher;

	/** @brief 게임 시작 버튼(이어하기) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAMenuButtonWidget> BtnContinue;
	
	/** @brief 게임 시작 버튼(처음부터) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAMenuButtonWidget> BtnNewGame;
	
	/** @brief 설정 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAMenuButtonWidget> BtnSettings;
	
	/** @brief 크레딧 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAMenuButtonWidget> BtnCredits;
	
	/** @brief 게임 종료 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URAMenuButtonWidget> BtnExitGame;

	/** @brief 스위처 안에 포함된 설정 메뉴 위젯 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAVolumeSettingsMenuWidget> VolumeSettingsMenu;

	/** @brief 곡 선택 위젯 참조 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URASongSelectWidget> SongSelectMenu;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- UI 바인딩 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 버튼 클릭 및 위젯 이벤트 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/**
	 * @brief 게임 시작 버튼(처음부터) 클릭 이벤트
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnNewGameClicked(int32 MenuIndex);

	/**
	 * @brief 설정 버튼 클릭 이벤트
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnSettingsClicked(int32 MenuIndex);
	
	/**	@brief 설정 메뉴가 닫힐 때 발생하는 이벤트 */
	UFUNCTION()
	void OnSettingsMenuClosed();

	UFUNCTION()
	void OnSongSelectMenuClosed();
	
	/** 
	 * @brief 게임 종료 버튼 클릭 이벤트
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnExitGameClicked(int32 MenuIndex);
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 버튼 클릭 및 위젯 이벤트 --- @/
	// ----------------------------------------------------------------------------------------------------------------
};
