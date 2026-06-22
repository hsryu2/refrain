// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_Attack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "Refrain.h"
#include "RefrainGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Refrain/Component/AttackTargetingComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"


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
	
	AvatarActor = Cast<ARACharacterPlayer>(ActorInfo->AvatarActor.Get());
	if (!AvatarActor)
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
	
	ClearAttackMotionWarpTarget();
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
	if (!AvatarActor)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	UAnimMontage* AttackMontage = AvatarActor->GetAttackMontage(CurrentCombo);
	if (!AttackMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	if (UAttackTargetingComponent* TargetingComponent = AvatarActor->FindComponentByClass<UAttackTargetingComponent>())
	{
		AActor* TargetActor = TargetingComponent->FindAttackTarget();
		
		if (TargetActor)
		{
			// 모션워핑 설정
			UpdateAttackMotionWarpTarget(TargetActor);
		}
		else
		{
			RA_LOG(LogRefrain, Log, TEXT("TargetActor Not Found"));
		}
	}
	else
	{
		RA_LOG(LogRefrain, Log, TEXT("TargetingComponent가 없습니다."));
	}
	
	// 공격 이펙트 실행
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Instigator = AvatarActor;
		CueParams.SourceObject = this;
		CueParams.Location = AvatarActor->GetActorLocation();
		CueParams.Normal = AvatarActor->GetActorForwardVector();
		
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

// 데미지 처리.
void URAGA_Attack::UpdateAttackMotionWarpTarget(AActor* TargetActor)
{
	if (!AvatarActor)
	{
		RA_LOG(LogRefrain, Error, TEXT("AvatarActor Not Found"));
		return;
	}
	
	UMotionWarpingComponent* MotionWarpingComponent = AvatarActor->FindComponentByClass<UMotionWarpingComponent>();
	if (!MotionWarpingComponent)
	{
		// 모션 워핑 컴포넌트가 없는 경우 그냥 바로 회전만
		RA_LOG(LogRefrain, Log, TEXT("MotionWarpingComponent Not Found"));
		FVector Direction = 
				TargetActor->GetActorLocation() - AvatarActor->GetActorLocation();
		Direction.Z = 0.0f;
		if (!Direction.IsNearlyZero())
		{
			FRotator TargetRotation = Direction.Rotation();
			AvatarActor->SetActorRotation(TargetRotation);
		}
	}
	else
	{
		
		ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
		if (!TargetCharacter)
		{
			RA_LOG(LogRefrain, Error, TEXT("TargetCharacter Cast Failed"));
			return;
		}
		USkeletalMeshComponent* TargetMesh = TargetCharacter->GetMesh();
		if (!TargetMesh)
		{
			RA_LOG(LogRefrain, Error, TEXT("TargetMesh Not Found"));
			return;
		}
		// 모션워핑에 필요한 정보 설정 (현재 오프셋 설정 안 됨)
		MotionWarpingComponent->AddOrUpdateWarpTargetFromComponent(
			FName(TEXT("Enemy")), TargetMesh, NAME_None, true, 
			EWarpTargetLocationOffsetDirection::VectorFromTargetToOwner);
	}
}

void URAGA_Attack::ClearAttackMotionWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComponent = AvatarActor->FindComponentByClass<UMotionWarpingComponent>();
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveAllWarpTargets();
	}
}

void URAGA_Attack::OnAttackHit(FGameplayEventData Payload)
{
	
	if (!AvatarActor || !DamageEffectClass)
	{
		return;
	}
	
	// 공격 타겟이 있는지 확인.
	AActor* TargetActor = nullptr;
	
	UAttackTargetingComponent* TargetingComponent =
		AvatarActor->FindComponentByClass<UAttackTargetingComponent>();
	
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
	
	UE_LOG(LogTemp, Warning, TEXT("Apply Damage: Target=%s Damage=%.1f"), *GetNameSafe(TargetActor), DamageAmount);
	if (!DamageEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageEffectClass is not assigned."));
		return;
	}
}
