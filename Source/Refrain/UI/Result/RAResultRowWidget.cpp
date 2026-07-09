// Fill out your copyright notice in the Description page of Project Settings.

#include "RAResultRowWidget.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"

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

	if (LabelSizeBox)
	{
		if (LabelWidth >= 0.0f)
		{
			LabelSizeBox->SetWidthOverride(LabelWidth);
		}
		else
		{
			LabelSizeBox->ClearWidthOverride();
		}
	}

	if (RowSpacer)
	{
		if (SpacerWidth >= 0.0f)
		{
			RowSpacer->SetSize(FVector2D(SpacerWidth, RowSpacer->GetSize().Y));
		}
	}
}

void URAResultRowWidget::SetValue(int32 InValue)
{
	if (ValueText)
	{
		ValueText->SetText(FText::AsNumber(InValue));
	}
}
