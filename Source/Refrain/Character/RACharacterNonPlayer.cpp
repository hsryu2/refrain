// Fill out your copyright notice in the Description page of Project Settings.


#include "RACharacterNonPlayer.h"

ARACharacterNonPlayer::ARACharacterNonPlayer()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

void ARACharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
