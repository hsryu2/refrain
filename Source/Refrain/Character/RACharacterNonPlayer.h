// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RACharacterBase.h"

// 6.21 현석 추가 -> NPC ASC, AttributeSet
#include "AbilitySystemInterface.h"
#include "GA/Attribute/RAAttributeSet.h"

#include "RACharacterNonPlayer.generated.h"

class UWidgetComponent;
class URhythmTargetWidget;
struct FStreamableHandle;
/**
 * @brief 플레이어가 아닌 캐릭터 클래스(적)\n
 * @note NPC의 메쉬는 UE의 기본적인 마네킹을 사용하기로 함\n
 * @date 2026-06-11\n
 * @author sejong
 */
UCLASS(Config=ArenaBattle)
class REFRAIN_API ARACharacterNonPlayer : public ARACharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ARACharacterNonPlayer();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	// --- override ---
	virtual void PostInitializeComponents() override;
	/** 블루프린트의 Construction Script와 동일한 역할 (에디터에서 값이 바뀌거나 배치될 때 실행됨) */
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void BeginPlay() override;
	
#if WITH_EDITOR
	/** 디테일 패널에서 수정을 감지하기 위한 함수 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	/** 타이밍 연출을 담당하기 위한 위젯 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	UWidgetComponent* RhythmTargetWidget;
	
	/** 위젯의 드로우 크기 (픽셀 단위, 예: 150, 150) */
	UPROPERTY(EditAnywhere, Category = "UI|Settings")
	FVector2D WidgetDrawSize = FVector2D(150.f, 150.f);
	
	/** 
	  * @brief: 에디터 프리뷰용 스케일 (월드 공간에서 보일 크기 조절) 
	  * @note: 인게임에는 영향을 주지 않으므로 값을 조절하여 상태를 확인합니다. 
	  */
	UPROPERTY(EditAnywhere, Category = "UI|Settings", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float WidgetEditorScale = 0.15f;
	
	/** 캐싱을 위한 인스턴스 포인터 */
	UPROPERTY()
	URhythmTargetWidget* RhythmWidgetInstance;
	
	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;
	
	TSharedPtr<FStreamableHandle> NPCMeshHandle;
	
	/** 공통 업데이트 로직 */
	void UpdateWidgetPreview();
	
	// GAS
private:
	UPROPERTY(VisibleAnywhere, Category="GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<URAAttributeSet> AttributeSet;
	
	// HP 위젯
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	UWidgetComponent* HealthWidget;
};
