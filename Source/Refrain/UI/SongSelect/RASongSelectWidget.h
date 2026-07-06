// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RASongSelectWidget.generated.h"

class UTextBlock;
class UImage;
class UListView;
class URAMenuButtonWidget;
class URASongInfoWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSongSelectClosed);

/**
 * @brief 곡 선택 화면을 제어하는 위젯 클래스
 * @note 블루프린트 위젯 생성 시 바인딩하여 사용합니다.
 */
UCLASS()
class REFRAIN_API URASongSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// /@ --- 외부 설정 (에디터 노출) ---
	
	/** @brief 에디터에서 할당할 곡 데이터 목록 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Select")
	TArray<TObjectPtr<UMagicalMusicData>> AvailableSongs;

	// --- 외부 설정 (에디터 노출) --- @/

	UPROPERTY(BlueprintAssignable, Category = "Song Select")
	FOnSongSelectClosed OnSongSelectClosed;

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

	/** @brief 리스트 뷰를 AvailableSongs 배열 기반으로 초기화합니다. */
	void InitSongList();

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
	TObjectPtr<URASongInfoWidget> SongInfoPanel;
	
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
