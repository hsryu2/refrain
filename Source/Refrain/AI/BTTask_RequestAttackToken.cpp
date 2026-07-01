// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_RequestAttackToken.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RACharacterNonPlayer.h"
#include "Character/RACharacterPlayer.h"
#include "Component/NPCCombatStateComponent.h"

UBTTask_RequestAttackToken::UBTTask_RequestAttackToken()
{
	NodeName = TEXT("Request Attack Token");
}

EBTNodeResult::Type UBTTask_RequestAttackToken::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	// BT를 실행중인 NPC 폰 가져오기.
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	
	ARACharacterNonPlayer* NPC = Cast<ARACharacterNonPlayer>(AIController->GetPawn());
	if (!NPC)
	{
		return EBTNodeResult::Failed;
	}
	
	// 블랙보드 가져오기.
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
		
	UObject* TargetObject = BlackboardComp->GetValueAsObject(TEXT("Player"));
	ARACharacterPlayer* Player = Cast<ARACharacterPlayer>(TargetObject);
	if (!Player)
	{
		return EBTNodeResult::Failed;
	}
	
	UNPCCombatStateComponent* CombatManager = Player->FindComponentByClass<UNPCCombatStateComponent>();
	if (!CombatManager)
	{
		return EBTNodeResult::Failed;
	}
	
	bool bHasToken = false;
	if (bIsCounterAttack)
	{
		bHasToken = CombatManager->RequestCounterAttackToken(NPC);
	}
	else
	{
		bHasToken = CombatManager->RequestMainAttackToken(NPC);
	}
	
	if (bHasToken)
	{
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
