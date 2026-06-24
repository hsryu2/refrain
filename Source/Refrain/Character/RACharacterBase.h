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
	ARACharacterBase();

// 재정의 함수
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	virtual URACharacterAnimationData* GetAnimationData() const { return AnimationData; }

	virtual void Die();
	
// 블루프린트에서 설정해줘야 하는 변수
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Refrain|Animation")
	URACharacterAnimationData* AnimationData;
	
protected:
	UPROPERTY(VisibleAnywhere, Category="GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;
};
