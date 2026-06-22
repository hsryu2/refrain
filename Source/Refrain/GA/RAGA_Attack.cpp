// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_Attack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "RefrainGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Refrain/Component/AttackTargetingComponent.h"

URAGA_Attack::URAGA_Attack()
{
	AbilityTags.AddTag(RefrainGameplayTags::Ability_Attack);
}

void URAGA_Attack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	PlayerCharacter = Cast<ARACharacterPlayer>(ActorInfo->AvatarActor.Get());
	if (!PlayerCharacter)
	{
		EndAbility(Handle,ActorInfo,ActivationInfo, true, true);
		return;
	}
	
	CurrentCombo = 1;
	bComboInput = false;
	bCanAcceptComboInput = false;
	
	PlayAttackMontage();
	
	// 콤보 시작 -> 콤보 예약 가능 구간
	UAbilityTask_WaitGameplayEvent* ComboStartTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			RefrainGameplayTags::Event_Montage_ComboStart,
			nullptr,
			false,
			true
		);
	
	ComboStartTask->EventReceived.AddDynamic(this, &URAGA_Attack::OnComboStart);
	ComboStartTask->ReadyForActivation();
	
	// 콤보 완료 -> 공격이 완료되거나 콤보 예약이 이후로 불가능.
	UAbilityTask_WaitGameplayEvent* ComboEndTask =
	UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		RefrainGameplayTags::Event_Montage_ComboEnd,
		nullptr,
		false,
		true
	);
	
	ComboEndTask->EventReceived.AddDynamic(this, &URAGA_Attack::OnComboEnd);
	ComboEndTask->ReadyForActivation();
	
	// 어택히트 이벤트 추가
	UAbilityTask_WaitGameplayEvent* AttackHitTask =
	UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		RefrainGameplayTags::Event_Montage_AttackHit,
		nullptr,
		false,
		true
	);
	
	AttackHitTask->EventReceived.AddDynamic(this, &URAGA_Attack::OnAttackHit);
	AttackHitTask->ReadyForActivation();
}

void URAGA_Attack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	if (bCanAcceptComboInput)
	{
		bComboInput = true;
	}
}

void URAGA_Attack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void URAGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URAGA_Attack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_Attack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URAGA_Attack::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}


void URAGA_Attack::OnComboStart(FGameplayEventData Payload)
{
	bCanAcceptComboInput = true;
}

void URAGA_Attack::OnComboEnd(FGameplayEventData Payload)
{
	bCanAcceptComboInput = false;
	
	if (bComboInput && CurrentCombo < MaxComboCount)
	{
		bComboInput = false;
		CurrentCombo++;
		
		PlayAttackMontage();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void URAGA_Attack::PlayAttackMontage()
{
	if (!PlayerCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	UAnimMontage* AttackMontage = PlayerCharacter->GetAttackMontage(CurrentCombo);
	if (!AttackMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	if (UAttackTargetingComponent* TargetingComponent = PlayerCharacter->FindComponentByClass<UAttackTargetingComponent>())
	{
		AActor* TargetActor = TargetingComponent->FindAttackTarget();
		
		if (TargetActor)
		{
			FVector Direction = 
				TargetActor->GetActorLocation() - PlayerCharacter->GetActorLocation();
			Direction.Z = 0.0f;
			if (!Direction.IsNearlyZero())
			{
				FRotator TargetRotation = Direction.Rotation();
				PlayerCharacter->SetActorRotation(TargetRotation);
			}
		}
	}
	
	// 공격 이펙트 실행
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Instigator = PlayerCharacter;
		CueParams.SourceObject = this;
		CueParams.Location = PlayerCharacter->GetActorLocation();
		CueParams.Normal = PlayerCharacter->GetActorForwardVector();
		
		ASC->ExecuteGameplayCue(RefrainGameplayTags::GameplayCue_Attack, CueParams);
	}
	
	// 현재 콤보에 따라서 Montage 실행
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			*FString::Printf(TEXT("AttackMontage_%d"), CurrentCombo),
			AttackMontage
		);
	// 몽타주가 완료/취소/중단 시 EndAbility를 호출하기 위해 델리게이트로 등록.
	MontageTask->OnCompleted.AddDynamic(this, &URAGA_Attack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URAGA_Attack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &URAGA_Attack::OnMontageCancelled);
	
	MontageTask->ReadyForActivation();
}

void URAGA_Attack::OnAttackHit(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("Attack Hit Event Received"));
	
	if (!PlayerCharacter || !DamageEffectClass)
	{
		return;
	}
	
	AActor* TargetActor = nullptr;
	
	UAttackTargetingComponent* TargetingComponent =
		PlayerCharacter->FindComponentByClass<UAttackTargetingComponent>();
	
	if (TargetingComponent)
	{
		TargetActor = TargetingComponent->FindAttackTarget();
	}
	
	if (!TargetActor)
	{
		return;
	}
	UAbilitySystemComponent* SourceASC =
		GetAbilitySystemComponentFromActorInfo();
	
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	if (!SourceASC || !TargetASC)
	{
		return;
	}
	FGameplayEffectSpecHandle DamageSpec =
		MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	
	if (!DamageSpec.IsValid())
	{
		return;
	}
	
	//DamageSpec.Data->SetByCallerTagMagnitudes(
	//	RefrainGameplayTags::Data_Damage,
	//	DamageAmount
	//);
	//
	//SourceASC
}
