// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatManagerComponent.h"

// Sets default values for this component's properties
UCombatManagerComponent::UCombatManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCombatManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UCombatManagerComponent::RequestMainAttackToken(ARACharacterNonPlayer* RequestingNPC)
{
	return true;
}

bool UCombatManagerComponent::RequestCounterAttackToken(ARACharacterNonPlayer* RequestingNPC)
{
	return true;
	
}

bool UCombatManagerComponent::ReleaseToken(ARACharacterNonPlayer* RequestingNPC)
{
	return true;
	
}

void UCombatManagerComponent::OnPlayerCounterSuccess()
{
}

void UCombatManagerComponent::RegisterNPC(ARACharacterNonPlayer* NPC)
{
}

void UCombatManagerComponent::UnRegisterNPC(ARACharacterNonPlayer* NPC)
{
}

