// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "RAAT_RhythmTargetWidgetProgress.generated.h"

class UWidgetComponent;
class URhythmTargetWidget;
/**
 * 
 */
UCLASS()
class REFRAIN_API URAAT_RhythmTargetWidgetProgress : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	static URAAT_RhythmTargetWidgetProgress* PlayRhythmTargetProgress(UGameplayAbility* OwningAbility, UWidgetComponent* InWidget, float Duration);
	
protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaSeconds) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	
protected:
	UPROPERTY()
	TObjectPtr<UWidgetComponent> RhythmTargetWidget;
	
	UPROPERTY()
	TObjectPtr<URhythmTargetWidget> RhythmTargetWidgetInstance;
	
private:
	float Duration = 0.f;
	float Elapsed = 0.f;
};
