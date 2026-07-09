// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/RASettingsSubsystem.h"
#include "SaveGame/RASettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Refrain/Refrain.h"

void URASettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadSettings();
}

void URASettingsSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void URASettingsSubsystem::InitializeAudioClasses(USoundMix* InMasterMix, USoundClass* InMasterClass, USoundClass* InBGMClass, USoundClass* InSFXClass)
{
	MasterSoundMix = InMasterMix;
	MasterSoundClass = InMasterClass;
	BGMSoundClass = InBGMClass;
	SFXSoundClass = InSFXClass;

	// 에셋 초기화가 끝났으므로, 로드된(또는 보류 중인) 볼륨 데이터를 기반으로 오디오 시스템에 적용
	ApplyAudioSettings();
}

void URASettingsSubsystem::TestMasterVolume(float NewVolume)
{
	PendingMasterVolume = FMath::Clamp(NewVolume, 0.0f, 1.0f);
	ApplyAudioSettings();
}

void URASettingsSubsystem::TestBGMVolume(float NewVolume)
{
	PendingBGMVolume = FMath::Clamp(NewVolume, 0.0f, 1.0f);
	ApplyAudioSettings();
}

void URASettingsSubsystem::TestSFXVolume(float NewVolume)
{
	PendingSFXVolume = FMath::Clamp(NewVolume, 0.0f, 1.0f);
	ApplyAudioSettings();
}

float URASettingsSubsystem::GetPendingMasterVolume() const
{
	return PendingMasterVolume;
}

float URASettingsSubsystem::GetPendingBGMVolume() const
{
	return PendingBGMVolume;
}

float URASettingsSubsystem::GetPendingSFXVolume() const
{
	return PendingSFXVolume;
}

void URASettingsSubsystem::SaveAudioSettings()
{
	if (CurrentSettings)
	{
		CurrentSettings->MasterVolume = PendingMasterVolume;
		CurrentSettings->BGMVolume = PendingBGMVolume;
		CurrentSettings->SFXVolume = PendingSFXVolume;
		SaveSettings();
	}
}

void URASettingsSubsystem::RevertAudioSettings()
{
	if (CurrentSettings)
	{
		PendingMasterVolume = CurrentSettings->MasterVolume;
		PendingBGMVolume = CurrentSettings->BGMVolume;
		PendingSFXVolume = CurrentSettings->SFXVolume;
		ApplyAudioSettings();
	}
}

void URASettingsSubsystem::ApplyAudioSettings()
{
	UWorld* World = GetWorld();
	if (!World || !MasterSoundMix)
	{
		return;
	}

	bool bPushed = false;

	if (MasterSoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(World, MasterSoundMix, MasterSoundClass, PendingMasterVolume, 1.0f, 0.0f, true);
		bPushed = true;
	}

	if (BGMSoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(World, MasterSoundMix, BGMSoundClass, PendingBGMVolume, 1.0f, 0.0f, true);
		bPushed = true;
	}

	if (SFXSoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(World, MasterSoundMix, SFXSoundClass, PendingSFXVolume, 1.0f, 0.0f, true);
		bPushed = true;
	}

	if (bPushed)
	{
		UGameplayStatics::PushSoundMixModifier(World, MasterSoundMix);
	}
}

void URASettingsSubsystem::LoadSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(SettingsSaveSlotName, 0))
	{
		USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SettingsSaveSlotName, 0);
		CurrentSettings = Cast<URASettingsSaveGame>(LoadedGame);
	}
	
	if (!CurrentSettings)
	{
		// 세이브 파일이 없거나 로드 실패 시 새로 생성
		CurrentSettings = Cast<URASettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(URASettingsSaveGame::StaticClass()));
	}

	// 로드 후 보류 중인 볼륨 변수 초기화
	if (CurrentSettings)
	{
		PendingMasterVolume = CurrentSettings->MasterVolume;
		PendingBGMVolume = CurrentSettings->BGMVolume;
		PendingSFXVolume = CurrentSettings->SFXVolume;
	}
}

void URASettingsSubsystem::SaveSettings()
{
	if (CurrentSettings)
	{
		UGameplayStatics::SaveGameToSlot(CurrentSettings, SettingsSaveSlotName, 0);
	}
}
