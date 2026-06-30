// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatManagerComponent.generated.h"

class ARACharacterPlayer;
class ARACharacterNonPlayer;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REFRAIN_API UCombatManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// NPC가 일반 공격 토큰을 요청할 때 호출
	UFUNCTION(BlueprintCallable, Category="Combat Token")
	bool RequestMainAttackToken(ARACharacterNonPlayer* RequestingNPC);
	
	// NPC가 카운터 가능한 공격 토큰을 요청할 때 호출.
	UFUNCTION(BlueprintCallable, Category="Combat Token")
	bool RequestCounterAttackToken(ARACharacterNonPlayer* RequestingNPC);
	
	// NPC가 일반 공격 토큰을 반환할 때 호출
	UFUNCTION(BlueprintCallable, Category="Combat Token")
	void ReleaseToken(ARACharacterNonPlayer* ReleasingNPC);
	
	// 플레이어가 카운터를 성공했을 때 호출. 
	UFUNCTION(BlueprintCallable, Category="Combat Token")
	void OnPlayerCounterSuccess();
	
	UFUNCTION(BlueprintCallable, Category="Combat Position")
	bool GetWaitLocation(ARACharacterNonPlayer* NPC, FVector& OutLocation);
	
	UPROPERTY(EditAnywhere, Category="CombatPosition")
	float WaitCircleRadius = 600.0f;
	
	void RegisterNPC(ARACharacterNonPlayer* NPC);
	void UnRegisterNPC(ARACharacterNonPlayer* NPC);
	
private:
	// 플레이어
	UPROPERTY()
	TObjectPtr<ARACharacterPlayer> OwnerPlayer;
	
	// 전투중인 NPC 목록
	UPROPERTY()
	TArray<ARACharacterNonPlayer*> EngagedNPCs;
	
	// 현재 공격 토큰을 쥐고 있는 NPC
	UPROPERTY()
	TObjectPtr<ARACharacterNonPlayer> CurrentMainAttacker;
	
	// 현재 카운터 공격 토큰을 쥐고있는 NPC
	UPROPERTY()
	TObjectPtr<ARACharacterNonPlayer> CurrentCounterAttacker;
};
