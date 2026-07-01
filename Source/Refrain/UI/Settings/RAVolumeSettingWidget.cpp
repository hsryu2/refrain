// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Settings/RAVolumeSettingWidget.h"
#include "Components/SLider.h"
#include "Components/TextBlock.h"

void URAVolumeSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 설정 이름 초기화
	if (TxtSettingName)
	{
		TxtSettingName->SetText(SettingName);
	}
	
	// 슬라이더 초기값 및 이벤트 바인딩
	if (SliderVolume)
	{
		SliderVolume->SetValue(InitialVolume);
		SliderVolume->OnValueChanged.AddDynamic(this, &URAVolumeSettingWidget::OnSliderValueChanged);
	}
	
	// 초기 텍스트 업데이트
	OnSliderValueChanged(InitialVolume);
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
	
	SliderVolume->SetValue(Value);
	
	// 이벤트 발행
	OnVolumeChanged.Broadcast(SettingName, Value);
}
