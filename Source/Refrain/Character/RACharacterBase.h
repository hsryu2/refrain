// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "RACharacterBase.generated.h"

class URACharacterAnimationData;

UCLASS()
class REFRAIN_API ARACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	virtual URACharacterAnimationData* GetAnimationData() const { return AnimationData; }

	virtual void Die();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Refrain|Animation")
	URACharacterAnimationData* AnimationData;
	
	UPROPERTY(VisibleAnywhere, Category="GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;
};
