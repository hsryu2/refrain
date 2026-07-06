// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RASongInfoWidget.generated.h"

class UTextBlock;
class UImage;
class UMagicalMusicData;

/**
 * @brief 곡의 기본 메타데이터(제목, 작곡가, BPM, 자켓)를 표시하는 재사용 가능한 공통 위젯 클래스
 */
UCLASS()
class REFRAIN_API URASongInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 전달받은 곡 데이터로 UI를 갱신합니다.
	 * @param InSongData 업데이트할 곡 데이터
	 */
	UFUNCTION(BlueprintCallable, Category = "Song Info")
	void UpdateSongInfo(UMagicalMusicData* InSongData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtSongTitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtArtist;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtBPM;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ImgSongJacket;
};
