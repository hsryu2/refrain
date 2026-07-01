// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_NPCAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "RefrainGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RACharacterNonPlayer.h"
#include "Character/RACharacterPlayer.h"
#include "Component/AttackTargetingComponent.h"
#include "Component/NPCCombatStateComponent.h"

URAGA_NPCAttack::URAGA_NPCAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URAGA_NPCAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	NPC = Cast<ARACharacterNonPlayer>(ActorInfo->AvatarActor.Get());
	// 스킬 발동 시: NPC의 타겟팅 위젯을 켭니다.
	if (NPC)
	{
		NPC->SetRhythmWidgetVisibility(true);
	}
	
	UAbilityTask_WaitGameplayEvent* AttackHitTask =
	UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		RefrainGameplayTags::Event_Montage_AttackHit,
		nullptr,
		false,
		true
	);

	AttackHitTask->EventReceived.AddDynamic(this, &URAGA_NPCAttack::OnAttackHit);
	AttackHitTask->ReadyForActivation();
	
	if (AttackMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("Attack"), AttackMontage); // 지정된 변수 사용

		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &URAGA_NPCAttack::OnMontageFinished);
			MontageTask->OnInterrupted.AddDynamic(this, &URAGA_NPCAttack::OnMontageFinished);
			MontageTask->ReadyForActivation();
		}
	}
	else
	{
		// 자산이 지정되지 않았을 때를 대비한 예외 처리
		UE_LOG(LogTemp, Warning, TEXT("AttackMontage is NOT set in %s"), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	
	
	// 애니메이션이 끝나면 꼭 EndAbility()를 호출해야 합니다.
}

void URAGA_NPCAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 스킬 종료 시: 위젯을 다시 끕니다.
	if (NPC)
	{
		NPC->SetRhythmWidgetVisibility(false);
	}
	
	// 토큰 반납.
	//if (AAIController* AIController = Cast<AAIController>(NPC->GetController()))
	//{
	//	if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
	//	{
	//		if (ARACharacterPlayer* Player = Cast<ARACharacterPlayer>(BlackboardComp->GetValueAsObject(TEXT("Player"))))
	//		{
	//			if (UNPCCombatStateComponent* CombatManager = Player->FindComponentByClass<UNPCCombatStateComponent>())
	//			{
	//				CombatManager->ReleaseToken(NPC);
	//			}
	//		}
	//	}
	//}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URAGA_NPCAttack::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_NPCAttack::OnAttackHit(FGameplayEventData Payload)
{
	
	if (!NPC || !DamageEffectClass)
	{
		return;
	}
	
	// 공격 타겟이 있는지 확인.
	//AActor* TargetActor = nullptr;
	
	TArray<AActor*> HitTargets;
	
	UAttackTargetingComponent* TargetingComponent =
		NPC->FindComponentByClass<UAttackTargetingComponent>();
	
	if (TargetingComponent)
	{
		HitTargets = TargetingComponent->HitSweep();
	}
	
	if (HitTargets.IsEmpty())
	{
		return;
	}
	
	// 플레이어와 타겟이 ASC를 가지고 있는지 확인.
	UAbilitySystemComponent* SourceASC =
		GetAbilitySystemComponentFromActorInfo();
	
	if (!SourceASC)
	{
		return;
	}
	
	for (AActor* TargetActor : HitTargets)
	{
		UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!TargetASC)
		{
			continue;
		}
		
		// 플레이어가 회피 상태라면 무시임. 아직 태그 상태를 확인 안해서 일단 주석처리.
		//if (TargetASC->HasMatchingGameplayTag(RefrainGameplayTags::State_Untargetable))
		//{
		//	UE_LOG(LogTemp, Log, TEXT("Target %s Perfect Dodged!"), *GetNameSafe(TargetActor));
		//	continue; // 이 타겟은 데미지 무시, 다음 타겟으로 넘어감
		//}
		
		FGameplayEffectSpecHandle DamageSpec = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
		if (DamageSpec.IsValid())
		{
			DamageSpec.Data->SetSetByCallerMagnitude(RefrainGameplayTags::Data_Damage, DamageAmount);
			SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpec.Data.Get(), TargetASC);
			UE_LOG(LogTemp, Log, TEXT("Apply Damage to %s"), *GetNameSafe(TargetActor));
		}
	}
}
