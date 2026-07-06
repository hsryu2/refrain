// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "RASongItemWidget.generated.h"

class UTextBlock;
class URASongItemData;

/**
 * @brief UListView의 각 항목(Row)에 표시될 위젯
 */
UCLASS()
class REFRAIN_API URASongItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtSongTitle;
};
