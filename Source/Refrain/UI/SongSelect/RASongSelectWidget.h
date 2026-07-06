// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RASongSelectWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UListView;

/**
 * @brief 곡 정보 구조체
 * @note 테스트 및 연동을 위해 임시로 정의된 구조체입니다.
 */
USTRUCT(BlueprintType)
struct FRASongInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Info")
	FString SongTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Info")
	FString Artist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Info")
	float BPM = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Info")
	TObjectPtr<class UTexture2D> JacketImage;
};

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
	 * @param InSongInfo 업데이트할 곡 정보
	 */
	UFUNCTION(BlueprintCallable, Category = "Song Select")
	void UpdateSongInfo(const FRASongInfo& InSongInfo);
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 블루프린트 연동 함수 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
protected:
	virtual void NativeConstruct() override;

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- UI 바인딩 ---
	// ----------------------------------------------------------------------------------------------------------------

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BtnBack;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BtnSettings;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BtnCategory;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BtnPlay;
	
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
	void OnBackClicked();
	
	UFUNCTION()
	void OnSettingsClicked();
	
	UFUNCTION()
	void OnCategoryClicked();
	
	UFUNCTION()
	void OnPlayClicked();
	
	UFUNCTION()
	void OnSongSelectionChanged(UObject* Item);
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 버튼 이벤트 --- @/
	// ----------------------------------------------------------------------------------------------------------------
};
