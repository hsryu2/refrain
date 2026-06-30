// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RAPauseMenuWidget.generated.h"

class URAMenuButtonWidget;
class ARAPlayerController;

/**
 * @brief 게임 내 일시정지 메뉴 위젯 클래스
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	int32 SelectedIndex = -1;

	UFUNCTION()
	void UpdateHighlightPosition(int32 MenuIndex);

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

	/** @brief 메인 메뉴로 이동할 맵 이름 (에디터 지정) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	UFUNCTION()
	void OnResumeClicked(int32 MenuIndex);

	UFUNCTION()
	void OnSettingsClicked(int32 MenuIndex);

	UFUNCTION()
	void OnMainMenuClicked(int32 MenuIndex);

	UFUNCTION()
	void OnExitGameClicked(int32 MenuIndex);
};
