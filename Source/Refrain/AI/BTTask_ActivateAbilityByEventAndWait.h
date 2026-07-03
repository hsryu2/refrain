// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "GameplayTagContainer.h"
#include "BTTask_ActivateAbilityByEventAndWait.generated.h"

class UAbilitySystemComponent;
class UBehaviorTreeComponent;
class UGameplayAbility;

/**
 * GameplayEvent payload로 GA를 실행하고 이 GA가 끝날 때 태스크를 종료하는 태스크
 */
UCLASS(Blueprintable, BlueprintType)
class REFRAIN_API UBTTask_ActivateAbilityByEventAndWait : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ActivateAbilityByEventAndWait();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	// 적용할 GA 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability)
	TSubclassOf<UGameplayAbility> AbilityClass;

	// GA ActivateAbility의 TriggerEventData로 전달할 이벤트 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability)
	FGameplayTag GameplayEventTag;

	// TriggerEventData.Target에 넣을 블랙보드 액터 키
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Blackboard)
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability)
	bool bCancelAbilityOnAbort = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability)
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
