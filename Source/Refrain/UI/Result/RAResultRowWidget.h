// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RAResultRowWidget.generated.h"

/**
 * @brief 게임 결과 항목(라벨과 숫자)을 한 줄로 표시하는 위젯
 * @note 상속받은 후 위젯을 배치할 때 디테일 패널에서 수치를 조정하세요.
 * @date 2026-07-06
 * @author Sejong
 */
UCLASS()
class REFRAIN_API URAResultRowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 
	  * @brief 숫자를 업데이트합니다.
	  * @param InValue 결과값(점수, 히트, ...등)
	 */
	UFUNCTION(BlueprintCallable, Category = "Result")
	void SetValue(int32 InValue);

protected:
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 텍스트 데이터 ---
	// ----------------------------------------------------------------------------------------------------------------

	/** @brief 실제 숫자가 표시될 텍스트 블록 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ValueText;

	/** @brief 라벨(항목 이름)이 표시될 텍스트 블록 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LabelText;
	
	/** @brief 에디터에서 직접 설정할 수 있는 항목 이름 
	  * @note 예: "Total Score"
	  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText RowTitle;

	/** @brief 에디터에서 설정할 수 있는 폰트 크기
	  * @note 기본값 24
	  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	int32 FontSize = 24;

	// ----------------------------------------------------------------------------------------------------------------
	// --- 텍스트 데이터 --- @/
	// ----------------------------------------------------------------------------------------------------------------

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 레이아웃 및 간격 설정 ---
	// ----------------------------------------------------------------------------------------------------------------

	/** @brief 라벨을 감싸는 사이즈 박스
	  * @note 선택 사항, 에디터에서 Is Variable 체크 및 이름을 LabelSizeBox로 지정해야 합니다.
	  */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class USizeBox> LabelSizeBox;

	/** @brief 라벨과 값 사이의 스페이서 
	  * @note 선택 사항, 에디터에서 Is Variable 체크 및 이름을 RowSpacer로 지정해야 합니다.
	  */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class USpacer> RowSpacer;

	/** @brief 라벨 가로 크기 오버라이드 
	  * @note 0보다 클 경우 적용, 기본값 -1.f는 무시
	  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Layout")
	float LabelWidth = -1.f;

	/** @brief 스페이서 가로 크기 오버라이드 
	  * @note 0보다 클 경우 적용, 기본값 -1.f는 무시
	  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Layout")
	float SpacerWidth = -1.f;

	// ----------------------------------------------------------------------------------------------------------------
	// --- 레이아웃 및 간격 설정 --- @/
	// ----------------------------------------------------------------------------------------------------------------

	
	virtual void NativePreConstruct() override;
};
