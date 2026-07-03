// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RASettingsSubsystem.generated.h"

class URASettingsSaveGame;

/**
 * @brief 설정 데이터를 메모리에 로드하고, 게임 중 오디오/환경 설정을 관리하는 서브시스템
 * @note GameInstanceSubsystem을 상속받으므로 게임 실행 내내 전역적으로 접근 가능합니다.
 * @date 2026-07-01
 * @author Sejong
 */
UCLASS()
class REFRAIN_API URASettingsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 서브시스템 생명주기 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 서브시스템 생명주기 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 오디오 설정 제어 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** 
	 * @brief 마스터 볼륨을 임시로 적용합니다 (저장 안 됨).
	 * @param NewVolume 0.0 ~ 1.0
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void TestMasterVolume(float NewVolume);

	/** 
	 * @brief BGM 볼륨을 임시로 적용합니다 (저장 안 됨).
	 * @param NewVolume 0.0 ~ 1.0
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void TestBGMVolume(float NewVolume);

	/** 
	 * @brief SFX 볼륨을 임시로 적용합니다 (저장 안 됨).
	 * @param NewVolume 0.0 ~ 1.0
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void TestSFXVolume(float NewVolume);

	/** @brief 현재 임시 마스터 볼륨 반환 */
	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetPendingMasterVolume() const;

	/** @brief 현재 임시 BGM 볼륨 반환 */
	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetPendingBGMVolume() const;

	/** @brief 현재 임시 SFX 볼륨 반환 */
	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetPendingSFXVolume() const;
	
	/** @brief 현재 임시 설정된 볼륨들을 디스크에 확정 저장합니다. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SaveAudioSettings();
	
	/** @brief 변경된 임시 볼륨들을 디스크에 저장된 원래 값으로 되돌립니다. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void RevertAudioSettings();

	/** @brief Pending 볼륨을 언리얼 오디오 시스템(SoundMix)에 일괄 적용합니다. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void ApplyAudioSettings();

	// ----------------------------------------------------------------------------------------------------------------
	// --- 오디오 설정 제어 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 사운드 에셋 레퍼런스 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 블루프린트에서 세팅할 오디오 에셋들을 초기화하기 위한 함수 */
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void InitializeAudioClasses(class USoundMix* InMasterMix, class USoundClass* InMasterClass, class USoundClass* InBGMClass, class USoundClass* InSFXClass);
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 사운드 에셋 레퍼런스 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
protected:
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 내부 로직 ---
	// ----------------------------------------------------------------------------------------------------------------
	
	/** @brief 디스크에서 설정을 불러옵니다. */
	void LoadSettings();

	/** @brief 디스크에 설정을 저장합니다. */
	void SaveSettings();
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 내부 로직 --- @/
	// ----------------------------------------------------------------------------------------------------------------
	
	// ----------------------------------------------------------------------------------------------------------------
	// /@ --- 내부 변수 ---
	// ----------------------------------------------------------------------------------------------------------------
	/** @brief 메모리에 로드된 세이브 객체 */
	UPROPERTY()
	TObjectPtr<URASettingsSaveGame> CurrentSettings;

	/** @brief 세이브 슬롯 이름 */
	const FString SettingsSaveSlotName = TEXT("SettingsSaveSlot");
	
	float PendingMasterVolume = 1.0f;
	float PendingBGMVolume = 1.0f;
	float PendingSFXVolume = 1.0f;
	
	/** @brief 볼륨 조절이 적용될 전체 사운드 믹스 */
	UPROPERTY()
	TObjectPtr<class USoundMix> MasterSoundMix;

	/** @brief 마스터 볼륨용 사운드 클래스 */
	UPROPERTY()
	TObjectPtr<class USoundClass> MasterSoundClass;

	/** @brief BGM 볼륨용 사운드 클래스 */
	UPROPERTY()
	TObjectPtr<class USoundClass> BGMSoundClass;

	/** @brief SFX 볼륨용 사운드 클래스 */
	UPROPERTY()
	TObjectPtr<class USoundClass> SFXSoundClass;
	
	// ----------------------------------------------------------------------------------------------------------------
	// --- 내부 변수 --- @/
	// ----------------------------------------------------------------------------------------------------------------
};
