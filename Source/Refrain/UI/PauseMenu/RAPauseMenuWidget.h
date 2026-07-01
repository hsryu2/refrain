// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RAPauseMenuWidget.generated.h"

class URAMenuButtonWidget;
class URAVolumeSettingsMenuWidget;
class ARAPlayerController;

/**
 * @brief 게임 내 일시정지 메뉴 위젯 클래스
 * @note 메뉴의 기능을 담당하며 위젯 블루프린트 작성시 바인딩하여 작성하도록 합니다.
 * @date 2026-07-01
 * @author Sejong
 */
UCLASS()
class REFRAIN_API URAPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** @brief 선택 효과 배경 위젯 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> SelectionHighlight;

	/** @brief 버튼 사이의 Y축 간격 */
	UPROPERTY(EditDefaultsOnly, Category = "UI|Animation")
	float MenuSpacing = 80.0f;

	float TargetTranslationY = 0.0f;
	float CurrentTranslationY = 0.0f;

	/** @brief 현재 선택된 퍼즈 메뉴 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	int32 SelectedIndex = -1;

	UFUNCTION()
	void UpdateHighlightPosition(int32 MenuIndex);

	/** @brief 메인 메뉴와 설정 화면 전환을 위한 최상위 스위치 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> MenuSwitcher;

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 퍼즈 메뉴 UI 바인딩 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 게임 계속하기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> Btn_Resume;

	/** @brief 설정 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> Btn_Settings;

	/** @brief 메인 메뉴로 돌아가기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> Btn_MainMenu;

	/** @brief 게임 종료 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> Btn_ExitGame;

	// ----------------------------------------------------------------------------------------------------------------
	// --- 퍼즈 메뉴 UI 바인딩 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 스위처 안에 포함된 설정 메뉴 위젯 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAVolumeSettingsMenuWidget> VolumeSettingsMenu;

	/** @brief 메인 메뉴로 이동할 맵 이름 (에디터 지정) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 버튼 상호작용 이벤트 ---
	// ----------------------------------------------------------------------------------------------------------------
	/**
	 * @brief 게임 계속하기(Resume) 버튼 클릭 이벤트
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnResumeClicked(int32 MenuIndex);

	/**
	 * @brief 설정 버튼 클릭 이벤트
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnSettingsClicked(int32 MenuIndex);

	/**	@brief 설정 메뉴가 닫힐 때 발생하는 이벤트 */
	UFUNCTION()
	void OnSettingsMenuClosed();

	/**
	 * @brief 메인 메뉴로 돌아가기(Main Menu) 버튼 클릭 이벤트
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnMainMenuClicked(int32 MenuIndex);

	/**
	 * @brief 게임 종료 버튼(Exit) 클릭 이벤트
	 * @param MenuIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnExitGameClicked(int32 MenuIndex);
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 버튼 상호작용 이벤트 --- @/
	// ----------------------------------------------------------------------------------------------------------------
};
