// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RACharacterBase.h"
#include "Animation/RACharacterAnimationData.h"
#include "AbilitySystemInterface.h"
#include "Player/RAPlayerState.h"
#include "RACharacterPlayer.generated.h"

class ARACharacterNonPlayer;

UCLASS()
class REFRAIN_API ARACharacterPlayer : public ARACharacterBase
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<class UInputAction> DodgeAction;
	
public:
	UAnimMontage* GetAttackMontage(int32 ComboIndex) const;
	UAnimMontage* GetDodgeMontage() const;
	
// 기본 동작
protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
//GAS
public:
	virtual void PossessedBy(AController* NewController) override;

protected:
	
	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;
	
	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartInputAbilities;
	
	//TObjectPtr<class UABCharacterAttributeSet> AttributeSet;
	
	void SetupGASInputComponent();	
	void GASInputPressed(int32 InputId);
	void GASInputReleased(int32 InputId);
	
// 타겟팅 컴포넌트
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Targeting)
	TObjectPtr<class UAttackTargetingComponent> TargetingComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=MotionWarping)
	TObjectPtr<class UMotionWarpingComponent> MotionWarpingComponent;
	
private:
	// 현재 타겟팅 중인 NPC
	UPROPERTY()
	TObjectPtr<ARACharacterNonPlayer> CurrentTarget;
	
	// 타겟이 변경되었을 때 처리하는 함수.
	void UpdateTarget(ARACharacterNonPlayer* NewTarget);
};
