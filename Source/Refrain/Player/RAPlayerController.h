// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "UI/RAHealthBarWidget.h"
#include "RAPlayerController.generated.h"

class URAPauseMenuWidget;
class URAScoreWidget;
class URAResultWidget;

/**
 * 
 */
UCLASS()
class REFRAIN_API ARAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ARAPlayerController();
	
	FORCEINLINE TObjectPtr<class UInputMappingContext> GetInputMappingContext() { return InputMappingContext; }

	void InitHealthHUD(UAbilitySystemComponent* InASC);
	
	/** @brief 일시정지 메뉴 토글 (ESC 입력시 호출) */
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void TogglePauseMenu();

	/** @brief 실제 게임 재개 (카운트인 완료 후 위젯에서 호출) */
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ExecuteUnpause();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	
	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<class UInputMappingContext> InputMappingContext;
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- HUD ---
	// ----------------------------------------------------------------------------------------------------------------
	
	// HP UI
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<URAHealthBarWidget> HealthBarWidgetClass;
	
	UPROPERTY()
	TObjectPtr<URAHealthBarWidget> HealthBarWidget;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<URAScoreWidget> ScoreWidgetClass;

	UPROPERTY()
	TObjectPtr<URAScoreWidget> ScoreWidget;

	// ----------------------------------------------------------------------------------------------------------------
	// --- HUD --- @/
	// ----------------------------------------------------------------------------------------------------------------

	//  /@ --- Pause UI ---
	/** @brief 일시정지 메뉴 위젯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<URAPauseMenuWidget> PauseMenuWidgetClass;

	/** @brief 일시정지 메뉴 위젯 */
	UPROPERTY()
	TObjectPtr<URAPauseMenuWidget> PauseMenuWidget;

	/** @brief 일시정지를 위한 키 매핑 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<class UInputAction> PauseAction;
	// --- Pause UI --- @/

	//  /@ --- Result UI ---
	/** @brief 결과 화면 위젯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<URAResultWidget> ResultWidgetClass;

	/** @brief 결과 화면 위젯 */
	UPROPERTY()
	TObjectPtr<URAResultWidget> ResultWidget;

public:
	/** @brief 게임 결과 화면 표시 (마우스 커서 활성화 및 입력 모드 변경) */
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowResultUI();

	/** @brief 게임 오버 화면 표시 (마우스 커서 활성화 및 입력 모드 변경) */
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowGameOverUI();

private:
	void InternalShowEndGameUI(bool bIsGameOver);
	// --- Result UI --- @/
};
