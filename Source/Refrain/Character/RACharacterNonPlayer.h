// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RACharacterBase.h"
#include "RACharacterNonPlayer.generated.h"

struct FStreamableHandle;
/**
 * @brief 플레이어가 아닌 캐릭터 클래스(적)\n
 * @note NPC의 메쉬는 UE의 기본적인 마네킹을 사용하기로 함\n
 * @date 2026-06-11\n
 * @author sejong
 */
UCLASS(Config=ArenaBattle)
class REFRAIN_API ARACharacterNonPlayer : public ARACharacterBase
{
	GENERATED_BODY()
	
public:
	ARACharacterNonPlayer();
	
protected:
	virtual void PostInitializeComponents() override;
	
	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;
	
	TSharedPtr<FStreamableHandle> NPCMeshHandle;
};
