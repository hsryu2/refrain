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
