// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Settings/RAVolumeSettingWidget.h"
#include "Components/SLider.h"
#include "Components/TextBlock.h"

void URAVolumeSettingWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// 설정 이름 초기화
	if (TxtSettingName)
	{
		TxtSettingName->SetText(SettingName);
	}
	
	// 이벤트 바인딩만 수행 (초기값 세팅은 부모 위젯이 UpdateSliderValue를 통해 직접 호출)
	if (SliderVolume)
	{
		SliderVolume->OnValueChanged.AddUniqueDynamic(this, &URAVolumeSettingWidget::OnSliderValueChanged);
	}
}

void URAVolumeSettingWidget::OnSliderValueChanged(float NewVolume)
{
	Value = NewVolume;
	
	// 0.0 ~ 1.0의 정규화 값을 0 ~ 100 퍼센트로 변환
	int32 Percentage = FMath::RoundToInt(Value * 100.0f);
	
	if (TxtCurrentVolume)
	{
		FString ValueString = FString::Printf(TEXT("%d%%"), Percentage);
		TxtCurrentVolume->SetText(FText::FromString(ValueString));
	}
	
	// 이벤트 발행
	OnVolumeChanged.Broadcast(SettingName, Value);
}

void URAVolumeSettingWidget::UpdateSliderValue(float SavedVolume)
{
	if (SliderVolume)
	{
		SliderVolume->SetValue(SavedVolume);
	}
	
	// 초기 텍스트 및 Value 갱신 (이벤트는 발행하지 않음, 무한루프 방지)
	Value = SavedVolume;
	int32 Percentage = FMath::RoundToInt(Value * 100.0f);
	
	if (TxtCurrentVolume)
	{
		FString ValueString = FString::Printf(TEXT("%d%%"), Percentage);
		TxtCurrentVolume->SetText(FText::FromString(ValueString));
	}
}
