// Fill out your copyright notice in the Description page of Project Settings.


#include "RAPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"

ARAPlayerController::ARAPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Game/Refrain/Input/IMC_Player.IMC_Player"));

	if (InputMappingContextRef.Object != nullptr)
	{
		InputMappingContext = InputMappingContextRef.Object;
	}
}

void ARAPlayerController::InitHealthHUD(UAbilitySystemComponent* InASC)
{
	if (!IsLocalController() || !InASC || !HealthBarWidgetClass)
	{
		return;
	}
	
	if (!HealthBarWidget)
	{
		HealthBarWidget = CreateWidget<URAHealthBarWidget>(
			this,
			HealthBarWidgetClass
		);
		
		if (HealthBarWidget)
		{
			HealthBarWidget->AddToViewport();
			
			HealthBarWidget->SetAlignmentInViewport(FVector2D(0.0f, 0.0f));
			HealthBarWidget->SetPositionInViewport(FVector2D(40.0f, 40.0f));
			HealthBarWidget->SetDesiredSizeInViewport(FVector2D(300.0f, 28.0f));
		}
	}
	
	if (HealthBarWidget)
	{
		HealthBarWidget->InitAbilitySystem(InASC);
	}
}

void ARAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
