// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetWaitLocation.h"

UBTTask_GetWaitLocation::UBTTask_GetWaitLocation()
{
	NodeName = TEXT("Get Wait Location");
}

EBTNodeResult::Type UBTTask_GetWaitLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
