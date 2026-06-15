// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGameMode.h"
#include "../Player/RAPlayerState.h"

ARAGameMode::ARAGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("//Game/Refrain/Player/BluePrint/BP_RACharacterPlayer.BP_RACharacterPlayer_C"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/Refrain.RAPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
	PlayerStateClass = ARAPlayerState::StaticClass();
}

void ARAGameMode::StartPlay()
{
	Super::StartPlay();
}
