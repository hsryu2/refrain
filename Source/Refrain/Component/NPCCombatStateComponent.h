// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NPCCombatStateComponent.generated.h"

class ARACharacterPlayer;
class ARACharacterNonPlayer;

/**
 * 플레이어 캐릭터에 붙어서 NPC들의 공격 토큰을 관리하고 배분하는 컴포넌트
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REFRAIN_API UNPCCombatStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNPCCombatStateComponent();

// 재정의 함수
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// NPC가 일반 공격 토큰을 요청할 때 호출
	UFUNCTION(BlueprintCallable, Category="Combat Token")
	bool RequestMainAttackToken(ARACharacterNonPlayer* RequestingNPC);
	
	// NPC가 카운터 가능한 공격 토큰을 요청할 때 호출.
	UFUNCTION(BlueprintCallable, Category="Combat Token")
	bool RequestCounterAttackToken(ARACharacterNonPlayer* RequestingNPC);
	
	// NPC가 일반 공격 토큰을 반환할 때 호출
	UFUNCTION(BlueprintCallable, Category="Combat Token")
	void ReleaseToken(ARACharacterNonPlayer* ReleasingNPC);
	
	UFUNCTION(BlueprintCallable, Category="Combat Position")
	bool GetWaitLocation(ARACharacterNonPlayer* NPC, FVector& OutLocation);
	
	void RegisterNPC(ARACharacterNonPlayer* NPC);
	void UnRegisterNPC(ARACharacterNonPlayer* NPC);
	
// Getter
public:
	ARACharacterNonPlayer* GetCurrentMainAttacker() { return CurrentMainAttacker; }
	ARACharacterNonPlayer* GetCurrentCounterAttacker() { return CurrentCounterAttacker; }
	
protected:
	UPROPERTY(EditAnywhere, Category="CombatPosition")
	float WaitCircleRadius = 600.0f;
	
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
