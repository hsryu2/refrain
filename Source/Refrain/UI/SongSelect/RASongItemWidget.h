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
 * @date 2026-07-01
 * @author Sejong
 */
UCLASS()
class REFRAIN_API URASongItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	// --- override ---
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** @brief 보간 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	float InterpSpeed = 12.0f;

	/** @brief 선택 시 커질 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	float SelectedScale = 1.2f;

	/** @brief 노래 제목 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtSongTitle;

private:
	/** @brief 애니메이션을 위한 현재 스케일 값 */
	float CurrentScale = 1.0f;
	
	/** @brief 목표 스케일 값 */
	float TargetScale = 1.0f;
};
