// Fill out your copyright notice in the Description page of Project Settings.

#include "RAResultRowWidget.h"
#include "Components/TextBlock.h"

void URAResultRowWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (LabelText)
	{
		LabelText->SetText(RowTitle);
		
		FSlateFontInfo FontInfo = LabelText->GetFont();
		FontInfo.Size = FontSize;
		LabelText->SetFont(FontInfo);
	}
	
	if (ValueText)
	{
		FSlateFontInfo FontInfo = ValueText->GetFont();
		FontInfo.Size = FontSize;
		ValueText->SetFont(FontInfo);
	}
}

void URAResultRowWidget::SetValue(int32 InValue)
{
	if (ValueText)
	{
		ValueText->SetText(FText::AsNumber(InValue));
	}
}
