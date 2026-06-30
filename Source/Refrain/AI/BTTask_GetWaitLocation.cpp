// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetWaitLocation.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RACharacterNonPlayer.h"
#include "Character/RACharacterPlayer.h"
#include "Component/CombatManagerComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_GetWaitLocation::UBTTask_GetWaitLocation()
{
	NodeName = TEXT("Get Wait Location");
}

EBTNodeResult::Type UBTTask_GetWaitLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
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
	
	ARACharacterPlayer* Player = Cast<ARACharacterPlayer>(UGameplayStatics::GetPlayerCharacter(NPC, 0));
	if (!Player)
	{
		return EBTNodeResult::Failed;
	}
	
	UCombatManagerComponent* CombatManager = Player->FindComponentByClass<UCombatManagerComponent>();
	if (!CombatManager)
	{
		return EBTNodeResult::Failed;
	}
	
	// 위치 계산.
	FVector TargetWaitLocation;
	if (CombatManager->GetWaitLocation(NPC, TargetWaitLocation))
	{
		// 계산된 위치를 블랙보드에 저장.
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		BlackboardComp->SetValueAsVector(WaitLocationKey.SelectedKeyName, TargetWaitLocation);
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}
