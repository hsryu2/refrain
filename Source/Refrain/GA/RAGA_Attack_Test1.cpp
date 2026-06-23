// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_Attack_Test1.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "Refrain.h"
#include "RefrainGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Refrain/Component/AttackTargetingComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "Animation/AnimNotify_SendGameplayEvent.h"
#include "Timing/MagicalTimingSubsystem.h"


URAGA_Attack_Test1::URAGA_Attack_Test1()
{
	FGameplayTagContainer Tags(RefrainGameplayTags::Ability_Attack);
	SetAssetTags(Tags);
}

void URAGA_Attack_Test1::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	RA_LOG(LogRefrain, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AvatarActor = Cast<ARACharacterPlayer>(ActorInfo->AvatarActor.Get());
	if (!AvatarActor)
	{
		EndAbility(Handle,ActorInfo,ActivationInfo, true, true);
		return;
	}
	
	CurrentCombo = 0;
	bComboInput = false;
	bCanAcceptComboInput = true;
	
	// PlayAttackMontage();
	
	// 콤보 시작 -> 콤보 예약 가능 구간
	UAbilityTask_WaitGameplayEvent* ComboStartTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			RefrainGameplayTags::Event_Montage_ComboInputStart,
			nullptr,
			false,
			true
		);
	
	ComboStartTask->EventReceived.AddDynamic(this, &URAGA_Attack_Test1::OnComboStart);
	ComboStartTask->ReadyForActivation();
	
	// 콤보 완료 -> 공격이 완료되거나 콤보 예약이 이후로 불가능.
	UAbilityTask_WaitGameplayEvent* ComboEndTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			RefrainGameplayTags::Event_Montage_ComboInputEnd,
			nullptr,
			false,
			true
		);
	
	ComboEndTask->EventReceived.AddDynamic(this, &URAGA_Attack_Test1::OnComboEnd);
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
	
	AttackHitTask->EventReceived.AddDynamic(this, &URAGA_Attack_Test1::OnAttackHit);
	AttackHitTask->ReadyForActivation();
	
	bIsAfterAttackHit = true;
}

void URAGA_Attack_Test1::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo)
{
	RA_LOG(LogRefrain, Log, TEXT("Start"));
	
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	if (bCanAcceptComboInput && !bComboInput)
	{
		bComboInput = true;
		bCanAcceptComboInput = false;
		
		if (CurrentCombo < MaxComboCount)
		{
			CurrentCombo++;
		}
		else if (CurrentCombo == MaxComboCount)
		{
			CurrentCombo = 1;
		}
	
		if (bIsAfterAttackHit)
		{
			PlayAttackMontage();
		}
	}
}

void URAGA_Attack_Test1::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void URAGA_Attack_Test1::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	RA_LOG(LogRefrain, Log, TEXT("EndAbility"));
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	ClearAttackMotionWarpTarget();
	TargetActor = nullptr;
	bIsAfterAttackHit = true;
}

void URAGA_Attack_Test1::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_Attack_Test1::OnMontageInterrupted()
{
	if (!bComboInput)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	else
	{
		bComboInput = false;
	}
}

void URAGA_Attack_Test1::OnMontageCancelled()
{
	if (!bComboInput)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	else
	{
		bComboInput = false;
	}
}


void URAGA_Attack_Test1::OnComboStart(FGameplayEventData Payload)
{
	bCanAcceptComboInput = true;
}

void URAGA_Attack_Test1::OnComboEnd(FGameplayEventData Payload)
{
	bCanAcceptComboInput = false;
}

void URAGA_Attack_Test1::OnAttackHit(FGameplayEventData Payload)
{
	bIsAfterAttackHit = true;
	
	if (!AvatarActor || !DamageEffectClass)
	{
		return;
	}

	if (!TargetActor)
	{
		RA_LOG(LogRefrain, Log, TEXT("빗나감"));
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
	}
	
	if (bComboInput)
	{
		PlayAttackMontage();
	}
}

void URAGA_Attack_Test1::PlayAttackMontage()
{
	if (!AvatarActor)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	// 다음 공격 몽타주
	UAnimMontage* AttackMontage = AvatarActor->GetAttackMontage(CurrentCombo);
	if (!AttackMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	// 타겟팅
	if (UAttackTargetingComponent* TargetingComponent = AvatarActor->FindComponentByClass<UAttackTargetingComponent>())
	{
		TargetActor = TargetingComponent->FindAttackTarget();
		
		if (TargetActor)
		{
			// 모션워핑 설정
			UpdateAttackMotionWarpTarget();
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
	
	const float BPM = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>()->GetBPM();
	const float AttackHitNotifyTime = FindGameplayEventNotifyTime(AttackMontage, RefrainGameplayTags::Event_Montage_AttackHit);
	const float EstimatedPlayRate = CalculateAttackPlayRate(AttackHitNotifyTime, 60.f / BPM * 0.9f);

	bIsAfterAttackHit = false;
	
	// 현재 콤보에 따라서 Montage 실행
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			*FString::Printf(TEXT("AttackMontage_%d"), CurrentCombo),
			AttackMontage,
			EstimatedPlayRate
		);
	
	// 몽타주가 완료/취소/중단 시 EndAbility를 호출하기 위해 델리게이트로 등록.
	MontageTask->OnCompleted.AddDynamic(this, &URAGA_Attack_Test1::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URAGA_Attack_Test1::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &URAGA_Attack_Test1::OnMontageCancelled);
	
	MontageTask->ReadyForActivation();
	
	bComboInput = false;
}

void URAGA_Attack_Test1::UpdateAttackMotionWarpTarget()
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

void URAGA_Attack_Test1::ClearAttackMotionWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComponent = AvatarActor->FindComponentByClass<UMotionWarpingComponent>();
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveAllWarpTargets();
	}
}

float URAGA_Attack_Test1::FindGameplayEventNotifyTime(const UAnimMontage* Montage, const FGameplayTag EventTag)
{
	if (!Montage)
	{
		return -1.f;
	}
	
	for (const FAnimNotifyEvent& NotifyEvent : Montage->Notifies)
	{
		const UAnimNotify_SendGameplayEvent* EventNotify = Cast<UAnimNotify_SendGameplayEvent>(NotifyEvent.Notify);
		if (EventNotify && EventNotify->EventTag == EventTag)
		{
			return NotifyEvent.GetTime();
		}
	}
	
	return -1.f;
}

float URAGA_Attack_Test1::CalculateAttackPlayRate(float NotifyTime, float MinimumStartupDelay)
{
	UMagicalTimingSubsystem* MagicalTiming = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>();
	if (!MagicalTiming)
	{
		RA_LOG(LogRefrain, Error, TEXT("MagicalTimingSubsystem Not Found"));
		return 1.f;
	}
	if (!MagicalTiming->IsMusicPlaying())
	{
		RA_LOG(LogRefrain, Warning, TEXT("Music is not playing"));
		return 1.f;
	}
	
	const float TargetTime = MagicalTiming->GetTimeUntilNextHit(MinimumStartupDelay);
	
	const float EstimatedPlayRate = NotifyTime / TargetTime;
	
	return EstimatedPlayRate;
}
