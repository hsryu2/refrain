// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RAMenuButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMenuButtonInteraction, int32, ButtonIndex);

class UTextBlock;

/**
 * @brief 메인 메뉴의 버튼을 담당하는 위젯 클래스\n
 * @note 위젯 블루프린트 작성시 바인딩하여 작성하도록 합니다.\n
 * @date 2026-06-29\n
 * @author sejong
 */
UCLASS()
class REFRAIN_API URAMenuButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	// --- 메인메뉴에서 발행할 이벤트  ---
	/** @brief 버튼 호버 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Menu")
	FOnMenuButtonInteraction OnMenuButtonHoveredEvent;
	
	/** @brief 버튼 클릭 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Menu")
	FOnMenuButtonInteraction OnMenuButtonClickedEvent;
	
	/** 
	 * @brief 메인 메뉴에서 버튼에 대한 상태를 처리하기 위한 이벤트 
	 * @notew 에디터에서 구현합니다.
	 * @param bIsSelected 선택되었는지 여부, True: 선택됨 / False: 선택 해제됨
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Menu")
	void OnSelectionStateChanged(bool bIsSelected);
	
	/** 
	 * @brief WBP를 사용하는 주체에서 텍스트를 변경하기 위한 변수 선언 
	 * @note 에디터에서 텍스트를 수정합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	FText ButtonText;
	
protected:
	// --- override ---
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	// --- 내부 버튼 이벤트를 받아줄 헬퍼 함수 ---
	
	/** @brief 버튼 호버 이벤트 받기 */
	UFUNCTION()
	void Internal_OnHovered();

	/** @brief 버튼 클릭 이벤트 받기 */
	UFUNCTION()
	void Internal_OnClicked();
		
	/** @brief 클릭을 받을 내부 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> MainMenuBtn;

	/** @brief 자신의 번호를 기억하기 위한 변수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	int32 ButtonIndex = 0;
	
	/** 화면에 표시엘 내부 텍스트 위젯 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ButtonTextBlock;
	
};
