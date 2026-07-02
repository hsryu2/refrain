// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/RAAT_RhythmTargetWidgetProgress.h"

#include "Refrain.h"
#include "Components/WidgetComponent.h"
#include "UI/RhythmTargetWidget.h"

URAAT_RhythmTargetWidgetProgress* URAAT_RhythmTargetWidgetProgress::PlayRhythmTargetProgress(UGameplayAbility* OwningAbility, UWidgetComponent* InWidget, float Duration)
{
	URAAT_RhythmTargetWidgetProgress* Task = NewAbilityTask<URAAT_RhythmTargetWidgetProgress>(OwningAbility);

	Task->bTickingTask = true;
	
	Task->RhythmTargetWidget = InWidget;
	Task->Duration = Duration;
	
	return Task;
}

void URAAT_RhythmTargetWidgetProgress::Activate()
{
	Super::Activate();
	
	if (!RhythmTargetWidget)
	{
		RA_LOG(LogRefrain, Error, TEXT("RhythmTargetWidget Not Found"));
		EndTask();
		return;
	}
	if (Duration <= 0.f)
	{
		RA_LOG(LogRefrain, Error, TEXT("Duration <= 0.f"));
		EndTask();
		return;
	}
	
	RhythmTargetWidgetInstance = Cast<URhythmTargetWidget>(RhythmTargetWidget->GetUserWidgetObject());
	if (!RhythmTargetWidgetInstance)
	{
		RA_LOG(LogRefrain, Error, TEXT("RhythmTargetWidget Cast Failed"));
		EndTask();
		return;
	}
	
	RhythmTargetWidgetInstance->UpdateProgress(0.f);
}

void URAAT_RhythmTargetWidgetProgress::TickTask(float DeltaSeconds)
{
	Super::TickTask(DeltaSeconds);
	
	if (!RhythmTargetWidget || Duration <= 0.f)
	{
		EndTask();
		return;
	}
	
	Elapsed += DeltaSeconds;

	const float Alpha = FMath::Clamp(Elapsed / Duration, 0.f, 1.f);
	RhythmTargetWidgetInstance->UpdateProgress(Alpha);

	if (Elapsed >= 1.1f)
	{
		EndTask();
	}
}

void URAAT_RhythmTargetWidgetProgress::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	
	RhythmTargetWidget->SetVisibility(false);
}
