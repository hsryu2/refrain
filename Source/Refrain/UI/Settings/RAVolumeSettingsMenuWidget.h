// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MainMenu/RAMenuButtonWidget.h"
#include "RAVolumeSettingsMenuWidget.generated.h"

class URAVolumeSettingWidget;
class URAMenuButtonWidget;
class USoundClass;
class USoundMix;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsMenuClosedSignature);

/**
 * @brief 볼륨을 조절하는 설정창 위젯
 * @note 마스터, BGM, SFX 등을 조절할 수 있습니다. 세부적으로 나누고 싶다면 사운드 클래스를 나누세요.
 * @author Sejong
 * @date 2026-07-01
 */
UCLASS()
class REFRAIN_API URAVolumeSettingsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 오디오 설정 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 볼륨 조절이 적용될 전체 사운드 믹스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Audio")
	TObjectPtr<USoundMix> MasterSoundMix;

	/** @brief 마스터 볼륨용 사운드 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Audio")
	TObjectPtr<USoundClass> MasterSoundClass;

	/** @brief BGM 볼륨용 사운드 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Audio")
	TObjectPtr<USoundClass> BGMSoundClass;

	/** @brief SFX 볼륨용 사운드 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Audio")
	TObjectPtr<USoundClass> SFXSoundClass;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 오디오 설정 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 설정창 이벤트 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 설정창이 닫힐 때 발생하는 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Settings|Event")
	FOnSettingsMenuClosedSignature OnSettingsMenuClosed;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 설정창 이벤트 --- @/
	// ----------------------------------------------------------------------------------------------------------------
protected:
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 설정 메뉴 UI 바인딩 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 마스터 볼륨 설정 슬라이더 위젯 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAVolumeSettingWidget> MasterVolumeWidget;

	/** @brief BGM 볼륨 설정 슬라이더 위젯 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAVolumeSettingWidget> BGMVolumeWidget;

	/** @brief SFX 볼륨 설정 슬라이더 위젯 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAVolumeSettingWidget> SFXVolumeWidget;

	/** @brief 창 닫기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> BtnClose;
	
	/** @brief 적용하기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> BtnApply;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 설정 메뉴 UI 바인딩 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 볼륨 변경 이벤트 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 마스터 볼륨 변경 이벤트 처리 */
	UFUNCTION()
	void OnMasterVolumeChanged(FText SettingName, float NewVolume);

	/** @brief BGM 볼륨 변경 이벤트 처리 */
	UFUNCTION()
	void OnBGMVolumeChanged(FText SettingName, float NewVolume);

	/** @brief SFX 볼륨 변경 이벤트 처리 */
	UFUNCTION()
	void OnSFXVolumeChanged(FText SettingName, float NewVolume);
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 볼륨 변경 이벤트 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 버튼 이벤트 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 적용 버튼 클릭 이벤트 처리 */
	UFUNCTION()
	void OnApplyClicked(int32 MenuIndex);
	
	/** @brief 닫기 버튼 클릭 이벤트 처리 */
	UFUNCTION()
	void OnCloseClicked(int32 MenuIndex);

	/** @brief 버튼 호버 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Menu")
	FOnMenuButtonInteraction OnMenuButtonHoveredEvent;
	
	/** @brief 버튼 호버 이벤트 내부 처리 */
	UFUNCTION()
	void OnButtonHovered(int32 MenuIndex);
	
	/** @brief 현재 선택된 버튼 인덱스 */
	UPROPERTY()
	int32 SelectedIndex = -1;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 버튼 클릭 이벤트 --- @/
	// ----------------------------------------------------------------------------------------------------------------
};
