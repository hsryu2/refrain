// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RhythmTargetWidget.generated.h"

class UImage;
class UUMGSequencePlayer;

/**
 * @class URhythmTargetWidget
 * @brief 리듬 타이밍에 맞춰 축소되는 링과 가이드 이미지를 표시하는 위젯
 * @note 일단 커밋 하고 수정하겠습니다. 파라미터값이 있긴 하지만 실제 블루프린트에서 연동하는 것은 아직 밝혀내지 못했습니다
 * @date 2026-06-15\n
 * @author sejong
 */
UCLASS()
class REFRAIN_API URhythmTargetWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	/** @brief 적 캐릭터의 타임라인에서 호출할 진행도 업데이트 함수: */
	UFUNCTION(BlueprintCallable, Category = "Rhythm System")
	void UpdateProgress(float Alpha);

	/** 성공/실패 시 이펙트 연출을 위한 함수 */
	UFUNCTION(BlueprintCallable, Category = "Rhythm System")
	void PlayFeedbackEffect(bool bIsSuccess);

protected:
	/** 디자이너의 Image 컴포넌트와 자동 바인딩 */
	UPROPERTY(meta = (BindWidget))
	UImage* RingTargetImage;
	
	/** 
	  * @brief: 바인딩 할 대상 Image \n
	  * @note: 만약 필요없는 경우라면 Image에서 Brush만 적용하지 않으면 됩니다 
	  */
	UPROPERTY(meta = (BindWidget))
	UImage* GuideImage;

	// 추가: 에디터 인스턴스나 블루프린트에서 쉽게 변경할 가이드 아이콘 텍스처
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm Settings")
	UTexture2D* GuideTexture;
	
	// 내부 제어용 다이내믹 머티리얼
	UPROPERTY()
	UMaterialInstanceDynamic* RingMaterialInstance;

	// --- 설정 파라미터 기본값 ---
	/** 움직이는 원의 색깔 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm Settings")
	FLinearColor MovingRingColor = FLinearColor::Blue;

	/** 목표 원의 색깔 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm Settings")
	FLinearColor GuideRingColor = FLinearColor::Gray;

	/** 목표 원의 반지름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm Settings")
	float GuideRingRadius = 0.2f;

};
