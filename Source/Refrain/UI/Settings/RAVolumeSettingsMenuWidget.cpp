// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Settings/RAVolumeSettingsMenuWidget.h"
#include "UI/Settings/RAVolumeSettingWidget.h"
#include "UI/MainMenu/RAMenuButtonWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

void URAVolumeSettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 바인딩 - 마스터 볼륨
	if (MasterVolumeWidget)
	{
		MasterVolumeWidget->OnVolumeChanged.AddDynamic(this, &URAVolumeSettingsMenuWidget::OnMasterVolumeChanged);
	}

	// 바인딩 - BGM 볼륨
	if (BGMVolumeWidget)
	{
		BGMVolumeWidget->OnVolumeChanged.AddDynamic(this, &URAVolumeSettingsMenuWidget::OnBGMVolumeChanged);
	}

	// 바인딩 - SFX 볼륨
	if (SFXVolumeWidget)
	{
		SFXVolumeWidget->OnVolumeChanged.AddDynamic(this, &URAVolumeSettingsMenuWidget::OnSFXVolumeChanged);
	}

	// 바인딩 - 닫기 버튼
	if (BtnClose)
	{
		BtnClose->OnMenuButtonClickedEvent.AddDynamic(this, &URAVolumeSettingsMenuWidget::OnCloseClicked);
	}
}

void URAVolumeSettingsMenuWidget::OnMasterVolumeChanged(FText SettingName, float NewVolume)
{
	if (MasterSoundMix && MasterSoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, MasterSoundClass, NewVolume, 1.0f, 0.0f, true);
		UGameplayStatics::PushSoundMixModifier(GetWorld(), MasterSoundMix);
	}
}

void URAVolumeSettingsMenuWidget::OnBGMVolumeChanged(FText SettingName, float NewVolume)
{
	if (MasterSoundMix && BGMSoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, BGMSoundClass, NewVolume, 1.0f, 0.0f, true);
		UGameplayStatics::PushSoundMixModifier(GetWorld(), MasterSoundMix);
	}
}

void URAVolumeSettingsMenuWidget::OnSFXVolumeChanged(FText SettingName, float NewVolume)
{
	if (MasterSoundMix && SFXSoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, SFXSoundClass, NewVolume, 1.0f, 0.0f, true);
		UGameplayStatics::PushSoundMixModifier(GetWorld(), MasterSoundMix);
	}
}

void URAVolumeSettingsMenuWidget::OnCloseClicked(int32 MenuIndex)
{
	// 설정창을 완전히 삭제하지 않고, 닫혔다는 신호만 부모 위젯으로 보냄
	OnSettingsMenuClosed.Broadcast();
}
