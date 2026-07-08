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
 * @date 2026-07-06
 * @author Sejong
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
	/** @brief 노래 제목 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtSongTitle;

	/** @brief 아티스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtArtist;

	/** @brief 노래의 BPM */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtBPM;

	/** @brief 곡을 대표하는 시각적 이미지 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ImgSongJacket;
	
	/** @brief 최고 점수(이전 기록) 표시 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URAResultRowWidget> HighScoreRow;

};
