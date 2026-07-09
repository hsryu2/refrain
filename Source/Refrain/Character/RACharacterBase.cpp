// Fill out your copyright notice in the Description page of Project Settings.


#include "RACharacterBase.h"

#include "AbilitySystemComponent.h"
#include "RefrainGameplayTags.h"

// Sets default values
ARACharacterBase::ARACharacterBase()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
}

void ARACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	// 설정된 값이 없을 때
	ensureMsgf(AnimationData, TEXT("AnimationData is not set on %s"), *GetNameSafe(this));
}

void ARACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* ARACharacterBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void ARACharacterBase::Die()
{
	ASC->AddLooseGameplayTag( RefrainGameplayTags::State_Dead);
	
}

