// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NPCCombatStateComponent.h"
#include "Character/RACharacterNonPlayer.h"
#include "Character/RACharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Refrain.h"
#include "Engine/Engine.h"

UNPCCombatStateComponent::UNPCCombatStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNPCCombatStateComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPlayer = Cast<ARACharacterPlayer>(GetOwner());
}

bool UNPCCombatStateComponent::RequestAttackToken(ARACharacterNonPlayer* RequestingNPC)
{
	if (CurrentAttacker == RequestingNPC)
	{
		return true;
	}
	
	// 누군가 공격중이라면 false
	if (CurrentAttacker != nullptr)
	{
		return false;
	}
	// 토큰 부여
	CurrentAttacker = RequestingNPC;
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT("%s 가 토큰 줍기 성공!"), *RequestingNPC->GetName()));
	// true로 반환하여 BT 혹은 공격 Ability를 실행하도록 할 수 있을 것으로 보임.
	return true;
}

bool UNPCCombatStateComponent::SetNowCounterableAttackTiming(ARACharacterNonPlayer* RequestingNPC, int32 Bar, float Beat)
{
	if (CurrentAttacker != RequestingNPC)
	{
		RA_LOG(LogRefrain, Error, TEXT("RequestingNPC != CurrentAttacker"));
		return false;
	}
	
	FAttackTiming NewAttackTiming = FAttackTiming(Bar, Beat);
	if (!NewAttackTiming.IsValid())
	{
		RA_LOG(LogRefrain, Warning, TEXT("AttackTiming Is Not Valid. Bar: %d, Beat: %f"), Bar, Beat);
		return false;
	}
	
	NowAttackTiming = &NewAttackTiming;
	
	return true;
}

bool UNPCCombatStateComponent::ClearNowCounterableAttackTiming(ARACharacterNonPlayer* RequestingNPC)
{
	if (CurrentAttacker != RequestingNPC)
	{
		RA_LOG(LogRefrain, Error, TEXT("RequestingNPC != CurrentAttacker"));
		return false;
	}
	
	NowAttackTiming = nullptr;
	return true;
}

void UNPCCombatStateComponent::ReleaseToken(ARACharacterNonPlayer* ReleasingNPC)
{
	if (CurrentAttacker == ReleasingNPC)
	{
		CurrentAttacker = nullptr;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("토큰 반납 완료!! 이제 남는 토큰 있음!"));
	}
}

bool UNPCCombatStateComponent::GetWaitLocation(ARACharacterNonPlayer* NPC, FVector& OutLocation)
{
	if (!OwnerPlayer || !NPC) return false;
	
	// 등록 되어있는 NPC인지 확인.
	int32 NPCIndex = EngagedNPCs.Find(NPC);
	
	// 등록되지 않은 NPC라면 임의로 추가.
	if (NPCIndex == INDEX_NONE)
	{
		RegisterNPC(NPC);
		NPCIndex = EngagedNPCs.Find(NPC);
	}
	
	// 겹치지 않게 자리 배분해주기.
	int32 TotalNPCs = EngagedNPCs.Num();
	if (TotalNPCs == 0) return false;
	
	float AngleDegree = (360.0f / TotalNPCs) * NPCIndex;
	
	FVector PlayerLocation = OwnerPlayer->GetActorLocation();
	FVector ForwardDir = OwnerPlayer->GetActorForwardVector();
	
	FVector RotatedDir = ForwardDir.RotateAngleAxis(AngleDegree, FVector::UpVector);
	
	OutLocation = PlayerLocation + (RotatedDir * WaitCircleRadius);
	
	return true;
}

void UNPCCombatStateComponent::RegisterNPC(ARACharacterNonPlayer* NPC)
{
	if (NPC && !EngagedNPCs.Contains(NPC))
	{
		EngagedNPCs.Add(NPC);
	}
}

void UNPCCombatStateComponent::UnRegisterNPC(ARACharacterNonPlayer* NPC)
{
	if (NPC)
	{
		EngagedNPCs.Remove(NPC);
		
		// 만약 토큰을 가지고있는 NPC가 죽거나 멀어지면 해제 해야함.
		ReleaseToken(NPC);
	}
}

