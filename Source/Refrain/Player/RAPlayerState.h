// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "RAPlayerState.generated.h"

UENUM(BlueprintType)
enum class ERAHitJudgement : uint8
{
	Perfect UMETA(DisplayName = "Perfect"),
	Good UMETA(DisplayName = "Good"),
	Bad UMETA(DisplayName = "Bad"),
	Miss UMETA(DisplayName = "Miss")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnScoreUpdate,
	ERAHitJudgement, Judgement,
	int32, AddedScore,
	int32, NewTotalScore,
	int32, NewCombo
);

/**
 */

UCLASS()
class REFRAIN_API ARAPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ARAPlayerState();
	virtual void BeginPlay() override;
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
public:
	// 점수시스템
	UPROPERTY(BlueprintReadOnly, Category = Score)
	int32 TotalScore = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = Score)
	int32 CurrentHits = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = Score)
	int32 MaxHits = 0;
	
	UFUNCTION(BlueprintCallable, Category = Score)
	void RegisterJudgement(ERAHitJudgement Judgement);
	
	UFUNCTION(BlueprintCallable, Category = Score)
	float GetHitsMultiplier() const;
	
	UFUNCTION(BlueprintCallable, Category = Score)
	void ResetHits();
	
	UPROPERTY(BlueprintAssignable, Category = Score)
	FOnScoreUpdate OnScoreUpdated;
	
	
	
protected:
	UPROPERTY(EditAnywhere, Category=GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<class URAAttributeSet> AttributeSet;

private:
	/** 플레이어 전용 스탯 초기화 GE */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Init", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> InitStatEffect;
	
};