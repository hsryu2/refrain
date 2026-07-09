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
 * @date 2026-07-01
 * @author Sejong
 */
UCLASS()
class REFRAIN_API URASongSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 외부 설정 (에디터 노출) ---
	// ----------------------------------------------------------------------------------------------------------------

	/** @brief 에디터에서 할당할 곡 데이터 목록 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Select")
	TArray<TObjectPtr<UMagicalMusicData>> AvailableSongs;

	/** @brief Play 버튼을 누를 때 이동할 실제 게임 레벨(맵) 지정 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TSoftObjectPtr<UWorld> GameplayLevel;

	// ----------------------------------------------------------------------------------------------------------------
	// --- 외부 설정 (에디터 노출) --- @/
	// ----------------------------------------------------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Song Select")
	FOnSongSelectClosed OnSongSelectClosed;

	/** @brief 현재 선택된 곡 데이터를 임시로 캐싱합니다. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Song Select")
	TObjectPtr<UMagicalMusicData> CurrentSelectedSong;

	/** @brief 현재 재생 중인 미리듣기 오디오 컴포넌트 */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Song Select")
	TObjectPtr<class UAudioComponent> PreviewAudioComponent;

	/** @brief 미리듣기 페이드 아웃을 위한 타이머 핸들 */
	FTimerHandle PreviewTimerHandle;

	/** @brief 미리듣기 재시작을 위한 타이머 핸들 */
	FTimerHandle PreviewRestartTimerHandle;

	/** @brief 미리듣기 오디오가 실제로 재생을 시작한 게임 시간(초) */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Song Select")
	float PreviewStartGameTime = 0.0f;

	/** @brief 미리듣기 오디오를 재생하고 페이드 인 하는 함수 */
	UFUNCTION()
	void StartPreviewAudio();

	/** @brief 미리듣기 시간이 끝나면 호출되어 오디오를 부드럽게 끕니다. */
	UFUNCTION()
	void OnPreviewFinished();

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 블루프린트 연동 함수 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/**
	 * @brief 현재 선택된 곡 정보를 UI에 업데이트합니다.
	 * @param InSongData 업데이트할 곡 데이터
	 */
	UFUNCTION(BlueprintCallable, Category = "Song Select")
	void UpdateSongInfo(class UMagicalMusicData* InSongData);

	/** @brief 메인 메뉴에서 이 화면으로 전환될 때 호출하여 소리 재생 등을 허용합니다. */
	UFUNCTION(BlueprintCallable, Category = "Song Select")
	void ActivateSongSelect();
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 블루프린트 연동 함수 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
protected:
	virtual void NativeConstruct() override;

	/** @brief 리스트 뷰를 AvailableSongs 배열 기반으로 초기화합니다. */
	void InitSongList();

	/** @brief 현재 위젯이 화면에 활성화되어 오디오 재생이 허용되는지 여부 */
	bool bIsPreviewAllowed = false;

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- UI 바인딩 ---
	// ----------------------------------------------------------------------------------------------------------------

	/** @brief 뒤로가기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> BtnBack;
	
	/** @brief 설정 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> BtnSettings;
	
	/** 
	 * @brief 카테고리 버튼 
	 * @note 디테일하게 하고 싶다면 추가 작성하세요.
	 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<URAMenuButtonWidget> BtnCategory;
	
	/** @brief 실행하기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URAMenuButtonWidget> BtnPlay;
	
	/** @brief 곡 정보를 띄워주는 패널 */
	UPROPERTY(BlueprintReadOnly, Category = "Song Select", meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<URASongInfoWidget> SongInfoPanel;
	
	/** @brief 노래 리스트 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UListView> SongListView;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- UI 바인딩 --- @/
	// ----------------------------------------------------------------------------------------------------------------

	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 버튼 이벤트 ---
	// ----------------------------------------------------------------------------------------------------------------

	/**
	 * @brief 뒤로가기 버튼 클릭 이벤트
	 * @param ButtonIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnBackClicked(int32 ButtonIndex);
	
	/**
	 * @brief 설정 버튼 클릭 이벤트
	 * @param ButtonIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnSettingsClicked(int32 ButtonIndex);
	
	/**
	 * @brief 카테코리 클릭 이벤트
	 * @param ButtonIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnCategoryClicked(int32 ButtonIndex);
	
	/**
	 * @brief 시작하기 버튼 클릭 이벤트
	 * @param ButtonIndex 버튼 인덱스
	 */
	UFUNCTION()
	void OnPlayClicked(int32 ButtonIndex);
	
	/**
	 * @brief 노래 선택 이벤트
	 * @param Item 버튼 인덱스
	 */
	UFUNCTION()
	void OnSongSelectionChanged(UObject* Item);
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 버튼 이벤트 --- @/
	// ----------------------------------------------------------------------------------------------------------------
};
