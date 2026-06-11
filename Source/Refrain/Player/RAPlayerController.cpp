// Fill out your copyright notice in the Description page of Project Settings.


#include "RAPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"

ARAPlayerController::ARAPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Game/Input/IMC_Player.IMC_Player"));

	if (InputMappingContextRef.Object != nullptr)
	{
		InputMappingContext = InputMappingContextRef.Object;
	}
}

void ARAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
