// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RACharacterBase.h"

#include "AbilitySystemInterface.h"
#include "../Player/RAPlayerState.h"
#include "RACharacterPlayer.generated.h"

UCLASS()
class REFRAIN_API ARACharacterPlayer : public ARACharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARACharacterPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Camera
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<class UCameraComponent> Camera;
	
// Input Section
protected:
	
	void SetIMC();
	UPROPERTY(EditAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<class UInputMappingContext> DefaultContext;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<class UInputAction> MoveAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<class UInputAction> LookAction;
	
protected:
	void Attack();
	
// 기본 동작
protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
//GAS
public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;
	
	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;
	
	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartInputAbilities;
	
	
	void SetupGASInputComponent();	
	void GASInputPressed(int32 InputId);
	void GASInputReleased(int32 InputId);
	
	
	
};
