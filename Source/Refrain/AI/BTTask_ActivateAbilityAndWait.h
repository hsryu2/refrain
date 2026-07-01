// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ActivateAbilityAndWait.generated.h"

class UAbilitySystemComponent;
class UBehaviorTreeComponent;
class UGameplayAbility;

/**
 * Activates a gameplay ability on the controlled pawn and keeps the BT task
 * running until that same ability ends.
 */
UCLASS()
class REFRAIN_API UBTTask_ActivateAbilityAndWait : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ActivateAbilityAndWait();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	UPROPERTY(EditAnywhere, Category=Ability)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, Category=Ability)
	bool bCancelAbilityOnAbort = true;

	UPROPERTY(EditAnywhere, Category=Ability)
	bool bTreatCancelledAbilityAsFailure = true;

private:
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
	void CleanupAbilityEndedDelegate();

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> CachedASC;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	FGameplayAbilitySpecHandle ActivatedAbilityHandle;
	FDelegateHandle AbilityEndedDelegateHandle;
	bool bWaitingForAbilityEnd = false;
	bool bActivatingAbility = false;
	bool bAbilityEndedDuringActivation = false;
	EBTNodeResult::Type AbilityEndResult = EBTNodeResult::Failed;
};
