// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <rapidjson/document.h>

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RAVolumeSettingWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVolumeChangedSignature, FText, SettingName, float, NewVolume);

/**
 * @brief 슬라이더를 통해 볼륨을 조절하는 설정 항목 위젯
 * @note 설정 이름과 변경된 볼륨 값을 함께 전달하기 위해 델리게이트를 사용하며 사운드를 세분화 하고 싶다면 사운드 클래스를 추가하세요.
 * @date 2026-07-01
 * @author Sejong
 */
UCLASS()
class REFRAIN_API URAVolumeSettingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 볼륨 설정 데이터 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 설정 항목 명칭 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ExposeOnSpawn))
	FText SettingName;
	
	/** @brief 초기 볼륨 값 (0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ExposeOnSpawn))
	float InitialVolume = 1.0f;
	
	FORCEINLINE float GetValue() const { return Value; }

	// ----------------------------------------------------------------------------------------------------------------
	// --- 볼륨 설정 데이터 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 볼륨 변경 이벤트 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 블루프린트에서 바인딩 할 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Settings|Event")
	FOnVolumeChangedSignature OnVolumeChanged;
	
	/**
	 * @brief 슬라이더 값이 변경될 때 호출되는 델리게이트 함수
	 * @param NewVolume 변경된 볼륨 값 (0.0 ~ 1.0 사이)
	 */
	UFUNCTION()
	void OnSliderValueChanged(float NewVolume);
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 볼륨 변경 이벤트 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
protected:
	virtual void NativeConstruct() override;
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 내부 로직 변수 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 변경된 볼륨 값 */
	float Value = 0.0f;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 내부 로직 변수 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- UI 바인딩 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 설정 항목의 이름 텍스트(BGM, SFX) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtSettingName;
	
	/** @brief 볼륨 조절 슬라이더 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USlider> SliderVolume;
	
	/** @brief 현재 볼륨 수치 표시 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtCurrentVolume;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- UI 바인딩 --- @/
	// ----------------------------------------------------------------------------------------------------------------
};
