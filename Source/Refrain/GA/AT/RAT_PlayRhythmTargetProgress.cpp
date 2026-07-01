// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/RAT_PlayRhythmTargetProgress.h"

#include "Refrain.h"
#include "Components/WidgetComponent.h"
#include "UI/RhythmTargetWidget.h"

URAT_PlayRhythmTargetProgress* URAT_PlayRhythmTargetProgress::PlayRhythmTargetProgress(UGameplayAbility* OwningAbility, UWidgetComponent* InWidget, float Duration)
{
	URAT_PlayRhythmTargetProgress* Task = NewAbilityTask<URAT_PlayRhythmTargetProgress>(OwningAbility);

	Task->bTickingTask = true;
	
	Task->RhythmTargetWidget = InWidget;
	Task->Duration = Duration;
	
	return Task;
}

void URAT_PlayRhythmTargetProgress::Activate()
{
	Super::Activate();
	
	if (!RhythmTargetWidget || Duration <= 0.f)
	{
		RA_LOG(LogRefrain, Error, TEXT("RhythmTargetWidget Not Found"));
		EndTask();
		return;
	}
	
	RhythmTargetWidgetInstance = Cast<URhythmTargetWidget>(RhythmTargetWidget->GetUserWidgetObject());
	
	RhythmTargetWidgetInstance->UpdateProgress(0.f);
}

void URAT_PlayRhythmTargetProgress::TickTask(float DeltaSeconds)
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

void URAT_PlayRhythmTargetProgress::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	
	RhythmTargetWidget->SetVisibility(false);
}
