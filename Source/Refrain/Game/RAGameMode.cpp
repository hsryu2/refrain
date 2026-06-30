// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGameMode.h"
#include "Player/RAPlayerController.h"
#include "Player/RAPlayerState.h"

ARAGameMode::ARAGameMode()
{
	PlayerControllerClass = ARAPlayerController::StaticClass();
	PlayerStateClass = ARAPlayerState::StaticClass();
}

void ARAGameMode::StartPlay()
{
	Super::StartPlay();
}
