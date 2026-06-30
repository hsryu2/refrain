// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatManagerComponent.h"
#include "Character/RACharacterNonPlayer.h"
#include "Character/RACharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

// Sets default values for this component's properties
UCombatManagerComponent::UCombatManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCombatManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UCombatManagerComponent::RequestMainAttackToken(ARACharacterNonPlayer* RequestingNPC)
{
	// 누군가 공격중이라면 false
	if (CurrentMainAttacker != nullptr)
	{
		return false;
	}
	// 토큰 부여
	CurrentMainAttacker = RequestingNPC;
	
	// true로 반환하여 BT 혹은 공격 Ability를 실행하도록 할 수 있을 것으로 보임.
	return true;
}

bool UCombatManagerComponent::RequestCounterAttackToken(ARACharacterNonPlayer* RequestingNPC)
{
	// 누군가 공격중이라면 false
	if (CurrentMainAttacker != nullptr)
	{
		return false;
	}
	// 토큰 부여
	CurrentCounterAttacker = RequestingNPC;
	
	// true로 반환하여 BT 혹은 공격 Ability를 실행하도록 할 수 있을 것으로 보임.
	return true;
}

void UCombatManagerComponent::ReleaseToken(ARACharacterNonPlayer* ReleasingNPC)
{
	if (CurrentMainAttacker == ReleasingNPC)
	{
		CurrentMainAttacker = nullptr;                                                                                    
	}
	else if (CurrentCounterAttacker == ReleasingNPC)
	{
		CurrentCounterAttacker = nullptr;                                                                                 
	}
}

void UCombatManagerComponent::OnPlayerCounterSuccess()
{
	// 카운터 토큰을 가진 적 처리
	if (CurrentCounterAttacker)
	{
		CurrentCounterAttacker = nullptr;
	}
	
	// 공격 중이었던 NPC 공격 중단
	if (CurrentMainAttacker)
	{
		if (UAbilitySystemComponent* NPC_ASC = CurrentMainAttacker->GetAbilitySystemComponent())
		{
			// 게임 플레이 태그로 공격중인 태그를 찾아서 취소.
			FGameplayTagContainer AttackTags;
			
			AttackTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Attacking.Main")));
			
			NPC_ASC->CancelAbilities(&AttackTags);
		}
		
		CurrentMainAttacker = nullptr;
	}
}

void UCombatManagerComponent::RegisterNPC(ARACharacterNonPlayer* NPC)
{
	if (NPC && !EngagedNPCs.Contains(NPC))
	{
		EngagedNPCs.Add(NPC);
	}
}

void UCombatManagerComponent::UnRegisterNPC(ARACharacterNonPlayer* NPC)
{
	if (NPC)
	{
		EngagedNPCs.Remove(NPC);
		
		// 만약 토큰을 가지고있는 NPC가 죽거나 멀어지면 해제 해야함.
		ReleaseToken(NPC);
	}
}

