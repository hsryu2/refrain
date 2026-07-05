// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NPCCombatStateComponent.generated.h"

class ARACharacterPlayer;
class ARACharacterNonPlayer;

USTRUCT(BlueprintType)
struct FAttackTiming
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int32 Bar = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	int32 Beat = -1;
	
	bool IsValid() const
	{
		return Bar >= 1 && Beat >= 1.f;
	}
};

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

public:
	// NPC가 공격 토큰을 요청할 때 호출
	UFUNCTION(BlueprintCallable, Category="Combat Token")
	bool RequestAttackToken(ARACharacterNonPlayer* RequestingNPC);
	
	// 공격 시 공격 타이밍 제출
	UFUNCTION(BlueprintCallable, Category="Combat")
	bool SetNowCounterableAttackTiming(ARACharacterNonPlayer* RequestingNPC, int32 Bar, int Beat);
	
	// 공격 후 공격 타이밍 초기화
	UFUNCTION(BlueprintCallable, Category="Combat")
	bool ClearNowCounterableAttackTiming(ARACharacterNonPlayer* RequestingNPC);
	
	// NPC가 공격 토큰을 반환할 때 호출
	UFUNCTION(BlueprintCallable, Category="Combat Token")
	void ReleaseToken(ARACharacterNonPlayer* ReleasingNPC);
	
	UFUNCTION(BlueprintCallable, Category="Combat Position")
	bool GetWaitLocation(ARACharacterNonPlayer* NPC, FVector& OutLocation);
	
	void RegisterNPC(ARACharacterNonPlayer* NPC);
	void UnRegisterNPC(ARACharacterNonPlayer* NPC);
	
// Getter
public:
	ARACharacterNonPlayer* GetCurrentAttacker() { return CurrentAttacker; }
	
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
	TObjectPtr<ARACharacterNonPlayer> CurrentAttacker;

	// 공격 중인 NPC가 공격할 타이밍
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CounterAttack")
	FAttackTiming NowAttackTiming;
};
