// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SongSelect/RASongItemWidget.h"
#include "Timing/MagicalMusicData.h"
#include "Components/TextBlock.h"

void URASongItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UMagicalMusicData* SongData = Cast<UMagicalMusicData>(ListItemObject);
	if (SongData && TxtSongTitle)
	{
		TxtSongTitle->SetText(FText::FromString(SongData->SongTitle));
	}
}

void URASongItemWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
	// 선택 여부에 따라 목표 스케일 변경
	TargetScale = bIsSelected ? SelectedScale : 1.0f;
}

void URASongItemWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 현재 스케일과 목표 스케일이 다르면 보간 진행
	if (!FMath::IsNearlyEqual(CurrentScale, TargetScale, 0.001f))
	{
		CurrentScale = FMath::FInterpTo(CurrentScale, TargetScale, InDeltaTime, InterpSpeed);
		SetRenderScale(FVector2D(CurrentScale, CurrentScale));
	}
}
