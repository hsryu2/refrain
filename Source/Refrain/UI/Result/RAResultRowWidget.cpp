// Fill out your copyright notice in the Description page of Project Settings.

#include "RAResultRowWidget.h"
#include "Components/TextBlock.h"

void URAResultRowWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	// 에디터에서 위젯을 배치할 때, Details 패널에 적은 RowTitle 글자를 바로 라벨 텍스트에 적용해 줍니다.
	if (LabelText)
	{
		LabelText->SetText(RowTitle);
	}
}

void URAResultRowWidget::SetValue(int32 InValue)
{
	if (ValueText)
	{
		ValueText->SetText(FText::AsNumber(InValue));
	}
}
