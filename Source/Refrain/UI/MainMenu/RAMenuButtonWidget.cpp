// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/RAMenuButtonWidget.h"
#include "Components/TextBlock.h"

#include "Components/Button.h"

void URAMenuButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	// 에디터에서도 텍스트가 바로 반영되도록 PreConstruct 사용
	if (ButtonTextBlock)
	{
		ButtonTextBlock->SetText(ButtonText);
	}
}

void URAMenuButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 실제 클릭/호버를 받을 버튼 위젯과 내부 함수 바인딩
	if (MainMenuBtn)
	{
		MainMenuBtn->OnHovered.AddUniqueDynamic(this, &URAMenuButtonWidget::Internal_OnHovered);
		MainMenuBtn->OnClicked.AddUniqueDynamic(this, &URAMenuButtonWidget::Internal_OnClicked);
	}
}

void URAMenuButtonWidget::Internal_OnHovered()
{
	// 내 번호를 담아서 메인 메뉴로 신호 전달
	OnMenuButtonHoveredEvent.Broadcast(ButtonIndex);
}

void URAMenuButtonWidget::Internal_OnClicked()
{
	// 내 번호를 담아서 메인 메뉴로 신호 전달
	OnMenuButtonClickedEvent.Broadcast(ButtonIndex);
}
