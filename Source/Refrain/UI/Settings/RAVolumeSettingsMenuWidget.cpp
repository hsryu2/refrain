// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Settings/RAVolumeSettingsMenuWidget.h"
#include "UI/Settings/RAVolumeSettingWidget.h"
#include "UI/MainMenu/RAMenuButtonWidget.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/RASettingsSubsystem.h"
#include "Engine/GameInstance.h"

void URAVolumeSettingsMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 바인딩 - 마스터 볼륨
	if (MasterVolumeWidget)
	{
		MasterVolumeWidget->OnVolumeChanged.AddUniqueDynamic(this, &URAVolumeSettingsMenuWidget::OnMasterVolumeChanged);
	}

	// 바인딩 - BGM 볼륨
	if (BGMVolumeWidget)
	{
		BGMVolumeWidget->OnVolumeChanged.AddUniqueDynamic(this, &URAVolumeSettingsMenuWidget::OnBGMVolumeChanged);
	}

	// 바인딩 - SFX 볼륨
	if (SFXVolumeWidget)
	{
		SFXVolumeWidget->OnVolumeChanged.AddUniqueDynamic(this, &URAVolumeSettingsMenuWidget::OnSFXVolumeChanged);
	}

	// 바인딩 - 닫기 버튼
	if (BtnClose)
	{
		BtnClose->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAVolumeSettingsMenuWidget::OnCloseClicked);
		BtnClose->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAVolumeSettingsMenuWidget::OnButtonHovered);
	}

	// 바인딩 - 적용 버튼
	if (BtnApply)
	{
		BtnApply->OnMenuButtonClickedEvent.AddUniqueDynamic(this, &URAVolumeSettingsMenuWidget::OnApplyClicked);
		BtnApply->OnMenuButtonHoveredEvent.AddUniqueDynamic(this, &URAVolumeSettingsMenuWidget::OnButtonHovered);
	}
	
	// Subsystem 초기화
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (URASettingsSubsystem* SettingsSubsystem = GameInstance->GetSubsystem<URASettingsSubsystem>())
		{
			// 오디오 에셋들 등록
			SettingsSubsystem->InitializeAudioClasses(MasterSoundMix, MasterSoundClass, BGMSoundClass, SFXSoundClass);
		}
	}
}

void URAVolumeSettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 메뉴가 열릴 때마다 저장된 설정(혹은 보류 중인 이전 설정)을 UI에 동기화
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (URASettingsSubsystem* SettingsSubsystem = GameInstance->GetSubsystem<URASettingsSubsystem>())
		{
			// ESC 등으로 강제 종료되었다가 다시 열렸을 때를 대비해 Revert 호출
			SettingsSubsystem->RevertAudioSettings();
			
			if (MasterVolumeWidget)
			{
				MasterVolumeWidget->UpdateSliderValue(SettingsSubsystem->GetPendingMasterVolume());
			}
			if (BGMVolumeWidget)
			{
				BGMVolumeWidget->UpdateSliderValue(SettingsSubsystem->GetPendingBGMVolume());
			}
			if (SFXVolumeWidget)
			{
				SFXVolumeWidget->UpdateSliderValue(SettingsSubsystem->GetPendingSFXVolume());
			}
		}
	}
}

void URAVolumeSettingsMenuWidget::OnMasterVolumeChanged(FText SettingName, float NewVolume)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (URASettingsSubsystem* SettingsSubsystem = GameInstance->GetSubsystem<URASettingsSubsystem>())
		{
			SettingsSubsystem->TestMasterVolume(NewVolume);
		}
	}
}

void URAVolumeSettingsMenuWidget::OnBGMVolumeChanged(FText SettingName, float NewVolume)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (URASettingsSubsystem* SettingsSubsystem = GameInstance->GetSubsystem<URASettingsSubsystem>())
		{
			SettingsSubsystem->TestBGMVolume(NewVolume);
		}
	}
}

void URAVolumeSettingsMenuWidget::OnSFXVolumeChanged(FText SettingName, float NewVolume)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (URASettingsSubsystem* SettingsSubsystem = GameInstance->GetSubsystem<URASettingsSubsystem>())
		{
			SettingsSubsystem->TestSFXVolume(NewVolume);
		}
	}
}

void URAVolumeSettingsMenuWidget::OnApplyClicked(int32 MenuIndex)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (URASettingsSubsystem* SettingsSubsystem = GameInstance->GetSubsystem<URASettingsSubsystem>())
		{
			SettingsSubsystem->SaveAudioSettings();
		}
	}
	
	// 아래 코드 활성화시 적용후 창이 닫아집니다
	// OnSettingsMenuClosed.Broadcast();
}

void URAVolumeSettingsMenuWidget::OnCloseClicked(int32 MenuIndex)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (URASettingsSubsystem* SettingsSubsystem = GameInstance->GetSubsystem<URASettingsSubsystem>())
		{
			// 저장 안 한 변경사항을 모두 원래대로 되돌림
			SettingsSubsystem->RevertAudioSettings();
			
			// 슬라이더 UI 동기화 (위젯 스위처로 다시 열었을 때 NativeConstruct가 불리지 않으므로)
			if (MasterVolumeWidget)
			{
				MasterVolumeWidget->UpdateSliderValue(SettingsSubsystem->GetPendingMasterVolume());
			}
			if (BGMVolumeWidget)
			{
				BGMVolumeWidget->UpdateSliderValue(SettingsSubsystem->GetPendingBGMVolume());
			}
			if (SFXVolumeWidget)
			{
				SFXVolumeWidget->UpdateSliderValue(SettingsSubsystem->GetPendingSFXVolume());
			}
		}
	}
	
	// 설정창을 완전히 삭제하지 않고, 닫혔다는 신호만 부모 위젯으로 보냄
	OnSettingsMenuClosed.Broadcast();
}

void URAVolumeSettingsMenuWidget::OnButtonHovered(int32 MenuIndex)
{
	OnMenuButtonHoveredEvent.Broadcast(MenuIndex);
}
