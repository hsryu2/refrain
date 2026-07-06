// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SongSelect/RASongItemWidget.h"
#include "UI/SongSelect/RASongItemData.h"
#include "Components/TextBlock.h"

void URASongItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	URASongItemData* SongData = Cast<URASongItemData>(ListItemObject);
	if (SongData && TxtSongTitle)
	{
		TxtSongTitle->SetText(FText::FromString(SongData->SongTitle));
	}
}
