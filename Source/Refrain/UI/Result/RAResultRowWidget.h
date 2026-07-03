// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RAResultRowWidget.generated.h"

/**
 * @brief 게임 결과 항목(라벨과 숫자)을 한 줄로 표시하는 위젯
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
	/** @brief 실제 숫자가 표시될 텍스트 블록 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ValueText;

	/** @brief 라벨(항목 이름)이 표시될 텍스트 블록 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LabelText;
	
	/** @brief 에디터에서 직접 설정할 수 있는 항목 이름 (예: "Total Score") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText RowTitle;

protected:
	virtual void NativePreConstruct() override;
};
