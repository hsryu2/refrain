// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetWaitLocation.generated.h"

/**
 * 
 */
UCLASS()
class REFRAIN_API UBTTask_GetWaitLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_GetWaitLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector WaitLocationKey;
};
