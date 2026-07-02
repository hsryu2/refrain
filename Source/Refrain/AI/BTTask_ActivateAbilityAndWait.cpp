// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ActivateAbilityAndWait.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Pawn.h"

UBTTask_ActivateAbilityAndWait::UBTTask_ActivateAbilityAndWait()
{
	NodeName = TEXT("Activate Ability And Wait");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_ActivateAbilityAndWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!AbilityClass)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledPawn);
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClass(AbilityClass);
	if (!AbilitySpec)
	{
		return EBTNodeResult::Failed;
	}

	CachedASC = ASC;
	CachedOwnerComp = &OwnerComp;
	ActivatedAbilityHandle = AbilitySpec->Handle;
	bWaitingForAbilityEnd = true;
	bActivatingAbility = true;
	bAbilityEndedDuringActivation = false;
	AbilityEndResult = EBTNodeResult::Failed;

	AbilityEndedDelegateHandle = ASC->OnAbilityEnded.AddUObject(this, &UBTTask_ActivateAbilityAndWait::OnAbilityEnded);

	const bool bActivated = ASC->TryActivateAbility(ActivatedAbilityHandle);
	bActivatingAbility = false;

	if (!bActivated)
	{
		CleanupAbilityEndedDelegate();
		return EBTNodeResult::Failed;
	}

	if (bAbilityEndedDuringActivation)
	{
		return AbilityEndResult;
	}

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_ActivateAbilityAndWait::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UAbilitySystemComponent* ASC = CachedASC;
	const FGameplayAbilitySpecHandle AbilityHandle = ActivatedAbilityHandle;
	const bool bShouldCancelAbility = bCancelAbilityOnAbort && ASC && AbilityHandle.IsValid();

	CleanupAbilityEndedDelegate();

	if (bShouldCancelAbility)
	{
		ASC->CancelAbilityHandle(AbilityHandle);
	}

	return EBTNodeResult::Aborted;
}

void UBTTask_ActivateAbilityAndWait::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	CleanupAbilityEndedDelegate();
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_ActivateAbilityAndWait::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (!bWaitingForAbilityEnd || AbilityEndedData.AbilitySpecHandle != ActivatedAbilityHandle)
	{
		return;
	}

	AbilityEndResult =
		AbilityEndedData.bWasCancelled && bTreatCancelledAbilityAsFailure
			? EBTNodeResult::Failed
			: EBTNodeResult::Succeeded;

	if (bActivatingAbility)
	{
		bAbilityEndedDuringActivation = true;
		CleanupAbilityEndedDelegate();
		return;
	}

	UBehaviorTreeComponent* OwnerComp = CachedOwnerComp.Get();
	CleanupAbilityEndedDelegate();

	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, AbilityEndResult);
	}
}

void UBTTask_ActivateAbilityAndWait::CleanupAbilityEndedDelegate()
{
	if (CachedASC && AbilityEndedDelegateHandle.IsValid())
	{
		CachedASC->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);
	}

	AbilityEndedDelegateHandle.Reset();
	ActivatedAbilityHandle = FGameplayAbilitySpecHandle();
	bWaitingForAbilityEnd = false;
	bActivatingAbility = false;
	CachedASC = nullptr;
	CachedOwnerComp = nullptr;
}
