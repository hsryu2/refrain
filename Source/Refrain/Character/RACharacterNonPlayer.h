// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RACharacterBase.h"

// 6.21 현석 추가 -> NPC ASC, AttributeSet
#include "AbilitySystemInterface.h"
#include "GA/Attribute/RAAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemComponent.h"


#include "RACharacterNonPlayer.generated.h"

class UWidgetComponent;
class URhythmTargetWidget;
struct FStreamableHandle;
/**
 * @brief 플레이어가 아닌 캐릭터 클래스(적)\n
 * @note NPC의 메쉬는 UE의 기본적인 마네킹을 사용하기로 함. 최대체력을 500, 방어력을 9로 설정하였음.\n
 * @date 2026-06-11\n
 * @author sejong
 */
UCLASS(Config=ArenaBattle)
class REFRAIN_API ARACharacterNonPlayer : public ARACharacterBase
{
	GENERATED_BODY()
	
public:
	ARACharacterNonPlayer();
	
	/**
	 * @brief RhythmTargetWidget을 띄우거나 띄우지 않는 함수
	 * @param bShow RhythmTarget을 띄울지 여부
	 * @note Quarts 시스템과 연결이 필요하기 때문에 현재 완벽하게 동작하지 않습니다. 다들 알다시피 온의 기준은 NPC의 공격 시도이며
	오프의 기준은 NPC의 공격이 다 끝날 때 등 입니다.\n 추가적으로, 아직은 공격을 할 떄 켜고 끄는 기능 밖에 없어 공격 속도와 동기화가 되어있거나 하지 않습니다!
	 */
	void SetRhythmWidgetVisibility(bool bShow);
	
	UWidgetComponent* GetHealthWidgetComponent() const {return HealthWidget;}
	
protected:
	// --- override ---
	virtual void PostInitializeComponents() override;
	/** 블루프린트의 Construction Script와 동일한 역할 (에디터에서 값이 바뀌거나 배치될 때 실행됨) */
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void BeginPlay() override;
	
	virtual void Die() override;
	
	FTimerHandle DeathTimerHandle;
	virtual void TriggerDissolve();
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
public:
	virtual void PossessedBy(AController* NewController) override;
	
private:
	UPROPERTY()
	TObjectPtr<URAAttributeSet> AttributeSet;
	
	/**
	 * @brief 캐릭터가 장착할 스킬을 블루프린트에서 세팅할 수 있도록 하는 배열(슬롯)
	 * @note UE 에디터에서 사용하고자 하는 기능을 지정해줘야 합니다.
	 */
	UPROPERTY(EditAnywhere, Category="GAS")
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;
	
	/** NPC 전용 스탯 초기화 GE */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Init", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> InitStatEffect;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Targeting, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAttackTargetingComponent> TargetingComponent;
	
	/**
	 * @brief 체력 속성 변경 시 호출되는 콜백 함수
	 * @param Data 변경된 속성 데이터
	 */
	virtual void OnHealthChanged(const FOnAttributeChangeData& Data);
	
protected:
	// HP 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	UWidgetComponent* HealthWidget;
};
