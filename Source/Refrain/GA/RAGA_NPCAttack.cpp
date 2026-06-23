// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_NPCAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "RefrainGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/RACharacterNonPlayer.h"
#include "Component/AttackTargetingComponent.h"

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
	AActor* TargetActor = nullptr;
	
	UAttackTargetingComponent* TargetingComponent =
		NPC->FindComponentByClass<UAttackTargetingComponent>();
	
	if (TargetingComponent)
	{
		TargetActor = TargetingComponent->FindAttackTarget();
	}
	
	if (!TargetActor)
	{
		return;
	}
	
	// 플레이어와 타겟이 ASC를 가지고 있는지 확인.
	UAbilitySystemComponent* SourceASC =
		GetAbilitySystemComponentFromActorInfo();
	
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	if (!SourceASC || !TargetASC)
	{
		return;
	}
	// GE Spec Handle이 있는지 확인.
	FGameplayEffectSpecHandle DamageSpec =
		MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	
	if (!DamageSpec.IsValid())
	{
		return;
	}
	// 위에 모든 조건이 통과되면 데미지 전달.
	DamageSpec.Data->SetSetByCallerMagnitude(
		RefrainGameplayTags::Data_Damage,
		DamageAmount
	);
	
	SourceASC->ApplyGameplayEffectSpecToTarget(
		*DamageSpec.Data.Get(),
		TargetASC
	);
	
	UE_LOG(LogTemp, Log, TEXT("Apply Damage: Target=%s Damage=%.1f"), *GetNameSafe(TargetActor), DamageAmount);
	if (!DamageEffectClass)
	{
		UE_LOG(LogTemp,Log, TEXT("DamageEffectClass is not assigned."));
		return;
	}
}
