// Fill out your copyright notice in the Description page of Project Settings.


#include "RAPlayerController.h"

void ARAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
