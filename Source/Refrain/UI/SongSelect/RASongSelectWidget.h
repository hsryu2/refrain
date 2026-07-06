// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RASongSelectWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UListView;
class URAMenuButtonWidget;
/**
 * @brief 곡 선택 화면을 제어하는 위젯 클래스
 * @note 블루프린트 위젯 생성 시 바인딩하여 사용합니다.
 */
UCLASS()
class REFRAIN_API URASongSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 블루프린트 연동 함수 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/**
	 * @brief 현재 선택된 곡 정보를 UI에 업데이트합니다.
	 * @param InSongData 업데이트할 곡 데이터
	 */
	UFUNCTION(BlueprintCallable, Category = "Song Select")
	void UpdateSongInfo(class UMagicalMusicData* InSongData);
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 블루프린트 연동 함수 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
protected:
	virtual void NativeConstruct() override;

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- UI 바인딩 ---
	// ----------------------------------------------------------------------------------------------------------------

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> BtnBack;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> BtnSettings;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<URAMenuButtonWidget> BtnCategory;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> BtnPlay;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtSongTitle;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtArtist;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtBPM;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ImgSongJacket;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UListView> SongListView;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- UI 바인딩 --- @/
	// ----------------------------------------------------------------------------------------------------------------

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 버튼 이벤트 ---
	// ----------------------------------------------------------------------------------------------------------------

	UFUNCTION()
	void OnBackClicked(int32 ButtonIndex);
	
	UFUNCTION()
	void OnSettingsClicked(int32 ButtonIndex);
	
	UFUNCTION()
	void OnCategoryClicked(int32 ButtonIndex);
	
	UFUNCTION()
	void OnPlayClicked(int32 ButtonIndex);
	
	UFUNCTION()
	void OnSongSelectionChanged(UObject* Item);
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 버튼 이벤트 --- @/
	// ----------------------------------------------------------------------------------------------------------------
};
