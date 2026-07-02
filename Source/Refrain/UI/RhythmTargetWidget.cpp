// Fill out your copyright notice in the Description page of Project Settings.


#include "RhythmTargetWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void URhythmTargetWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RingTargetImage)
	{
		// 머티리얼 인스턴스 동적 생성 및 적용
		RingMaterialInstance = RingTargetImage->GetDynamicMaterial();
        
		if (RingMaterialInstance)
		{
			// 기획에 맞게 초기 파라미터 세팅
			RingMaterialInstance->SetVectorParameterValue(TEXT("MovingRingColor"), MovingRingColor);
			RingMaterialInstance->SetVectorParameterValue(TEXT("GuideRingColor"), GuideRingColor);
			RingMaterialInstance->SetScalarParameterValue(TEXT("GuideRingRadius"), GuideRingRadius);
            
			// 시작할 때는 링이 완전히 바깥에 있도록 초기화 (Alpha = 0)
			RingMaterialInstance->SetScalarParameterValue(TEXT("SyncProgress"), 0.0f);
		}
	}
}

void URhythmTargetWidget::UpdateProgress(float Alpha)
{
	if (RingMaterialInstance)
	{
		// 적 캐릭터의 타임라인 값(0.0 ~ 1.0)을 받아 머티리얼 링을 축소시킴
		RingMaterialInstance->SetScalarParameterValue(TEXT("AttackProgressParam"), Alpha);
	}
}

void URhythmTargetWidget::PlayFeedbackEffect(bool bIsSuccess)
{
	// todo: 사용자 입력 피드백 구현
	/*
	if (bIsSuccess)
	{
		// 성공 시 링이 팡 터지거나 녹색으로 번쩍이는 위젯 애니메이션 재생
		PlayAnimation(SuccessAnim);
	}
	else
	{
		// 실패 시 링이 붉게 변하며 흐려지는 등의 예외 처리 로직
	}
	*/
}
