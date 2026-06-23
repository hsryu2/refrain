// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_Attack_Test2.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "Refrain.h"
#include "RefrainGameplayTags.h"
#include "Animation/AnimNotify_SendGameplayEvent.h"
#include "Animation/RACharacterAnimationData.h"
#include "Character/RACharacterBase.h"
#include "Component/AttackTargetingComponent.h"
#include "Timing/MagicalTimingSubsystem.h"

class UMotionWarpingComponent;

URAGA_Attack_Test2::URAGA_Attack_Test2()
{
	FGameplayTagContainer Tags(RefrainGameplayTags::Ability_Attack);
	SetAssetTags(Tags);;
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URAGA_Attack_Test2::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 변수 초기화
	RA_LOG(LogRefrain, Log, TEXT("Start"));
	AvatarCharacter = Cast<ARACharacterBase>(ActorInfo->AvatarActor.Get());
	if (!AvatarCharacter)
	{
		RA_LOG(LogRefrain, Error, TEXT("AvatarCharacter Not Found"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	TargetingComponent = AvatarCharacter->FindComponentByClass<UAttackTargetingComponent>();
	CurrentCombo = 0;
	
	PlayAttackMontage();
}

void URAGA_Attack_Test2::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	SetJudgement();
	CurrentCombo++;
}

void URAGA_Attack_Test2::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	ClearAttackMotionWarpTarget();
	TargetActor = nullptr;
}

void URAGA_Attack_Test2::OnAttackMontageEnded(UAnimMontage* AnimMontage, const bool bInterrupted)
{
}

void URAGA_Attack_Test2::OnAttackHit()
{
}

void URAGA_Attack_Test2::PlayAttackMontage()
{
	// 다음 공격 몽타주
	UAnimMontage* AttackMontage = GetNextAttackAnimMontage();
	if (!AttackMontage)
	{
		RA_LOG(LogRefrain, Warning, TEXT("AttackMontage Not Found"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	// 타겟팅
	if (!TargetActor || )
	{
		TargetActor = TargetingComponent->FindAttackTarget();
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

UAnimMontage* URAGA_Attack_Test2::GetNextAttackAnimMontage() const
{
	const ARACharacterBase* RACharacter = Cast<ARACharacterBase>(AvatarCharacter);
	const URACharacterAnimationData* AnimationData = RACharacter->GetAnimationData();
	check(AnimationData);
	
	if (AnimationData->AttackMontages.IsEmpty())
	{
		RA_LOG(LogRefrain, Warning, TEXT("AttackMontages Array Empty"));
		return AnimationData->AttackMontage_1;
	}
	
	const int MontageArrayNum = AnimationData->AttackMontages.Num();
	
	return AnimationData->AttackMontages[CurrentCombo % MontageArrayNum];
}

void URAGA_Attack_Test2::UpdateAttackMotionWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComponent = AvatarCharacter->FindComponentByClass<UMotionWarpingComponent>();
	if (!MotionWarpingComponent)
	{
		// 모션 워핑 컴포넌트가 없는 경우 그냥 바로 회전만
		RA_LOG(LogRefrain, Log, TEXT("MotionWarpingComponent Not Found"));
		FVector Direction = 
			TargetActor->GetActorLocation() - AvatarCharacter->GetActorLocation();
		Direction.Z = 0.0f;
		if (!Direction.IsNearlyZero())
		{
			FRotator TargetRotation = Direction.Rotation();
			AvatarCharacter->SetActorRotation(TargetRotation);
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

void URAGA_Attack_Test2::ClearAttackMotionWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComponent = AvatarCharacter->FindComponentByClass<UMotionWarpingComponent>();
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveAllWarpTargets();
	}
}

float URAGA_Attack_Test2::FindGameplayEventNotifyTime(const UAnimMontage* Montage, const FGameplayTag EventTag) const
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

float URAGA_Attack_Test2::CalculateAttackPlayRate(float NotifyTime, float MinimumStartupDelay) const
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

void URAGA_Attack_Test2::SetTargetActor()
{
	if (!TargetActor)
	{
		TargetActor = TargetingComponent->FindAttackTarget();
		return;
	}
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		RA_LOG(LogRefrain, Warning, TEXT("TargetASC Not Found From TargetActor"));
		return;
	}
	
	if (TargetASC->HasMatchingGameplayTag(RefrainGameplayTags::State_Dead))
	{
		TargetActor = TargetingComponent->FindAttackTarget();
	}
}

void URAGA_Attack_Test2::SetJudgement()
{
	UMagicalTimingSubsystem* MagicalTiming = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>();
	if (!MagicalTiming)
	{
		RA_LOG(LogRefrain, Error, TEXT("MagicalTimingSubsystem Not Found"));
		return;
	}
	const float TimingDifference = MagicalTiming->JudgeTiming();
	
	if (!TargetActor)
	{
		JudgementTag = RefrainGameplayTags::Judge_Miss;
	}
	else if (TimingDifference < 0.05f)
	{
		JudgementTag = RefrainGameplayTags::Judge_Perfect;
	}
	else if (TimingDifference < 0.2f)
	{
		JudgementTag = RefrainGameplayTags::Judge_Good;
	}
	else
	{
		JudgementTag = RefrainGameplayTags::Judge_Bad;
	}
}
