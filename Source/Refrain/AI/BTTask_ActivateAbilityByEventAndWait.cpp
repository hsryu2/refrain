// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ActivateAbilityByEventAndWait.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

UBTTask_ActivateAbilityByEventAndWait::UBTTask_ActivateAbilityByEventAndWait()
{
	NodeName = TEXT("Activate Ability By Event And Wait");
	bCreateNodeInstance = true;

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ActivateAbilityByEventAndWait, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_ActivateAbilityByEventAndWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!AbilityClass || !GameplayEventTag.IsValid() || TargetActorKey.SelectedKeyName.IsNone())
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
	if (!ASC->AbilityActorInfo.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AActor* TargetActor = BlackboardComp
		? Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName))
		: nullptr;
	if (!TargetActor)
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

	AbilityEndedDelegateHandle = ASC->OnAbilityEnded.AddUObject(this, &UBTTask_ActivateAbilityByEventAndWait::OnAbilityEnded);

	FGameplayEventData Payload;
	Payload.EventTag = GameplayEventTag;
	Payload.Instigator = ControlledPawn;
	Payload.Target = TargetActor;

	const bool bActivated = ASC->TriggerAbilityFromGameplayEvent(
		ActivatedAbilityHandle,
		ASC->AbilityActorInfo.Get(),
		GameplayEventTag,
		&Payload,
		*ASC);
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

EBTNodeResult::Type UBTTask_ActivateAbilityByEventAndWait::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

void UBTTask_ActivateAbilityByEventAndWait::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	CleanupAbilityEndedDelegate();
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_ActivateAbilityByEventAndWait::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
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

void UBTTask_ActivateAbilityByEventAndWait::CleanupAbilityEndedDelegate()
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
