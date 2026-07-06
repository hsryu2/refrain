// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "NiagaraSystem.h"
#include "RAGC_Attack.generated.h"

/**
 * 
 */
UCLASS()
class REFRAIN_API URAGC_Attack : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	URAGC_Attack();

protected:
	// GameplayCue가 Excute 방식으로 실행했을 때 호출되는 함수.
	virtual bool OnExecute_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters
	) const override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect)
	TObjectPtr<UNiagaraSystem> AttackEffect;
	
	// 소켓은
	// HandGrip_L
	// HandGrip_R
	// foot_l_Socket
	// foot_r_Socket
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect)
	TMap<FGameplayTag, FName> AttackSocketMap;
};
