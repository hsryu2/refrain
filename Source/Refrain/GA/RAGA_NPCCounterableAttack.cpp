// Fill out your copyright notice in the Description page of Project Settings.


#include "RAGA_NPCCounterableAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "Refrain.h"
#include "RefrainGameplayTags.h"
#include "TimerManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AN_SendGameplayEvent.h"
#include "AT/RAAT_RhythmTargetWidgetProgress.h"
#include "Character/RACharacterNonPlayer.h"
#include "Character/RACharacterPlayer.h"
#include "Component/AttackHitSweepComponent.h"
#include "Component/NPCCombatStateComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Timing/MagicalTimingSubsystem.h"
#include "UI/RhythmTargetWidget.h"
#include "Util/RAUtils.h"

URAGA_NPCCounterableAttack::URAGA_NPCCounterableAttack()
{
	FGameplayTagContainer Tags(RefrainGameplayTags::State_Attacking_Counterable);
	SetAssetTags(Tags);
	
	ActivationOwnedTags.AddTag(RefrainGameplayTags::State_Attacking_Counterable);
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URAGA_NPCCounterableAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 변수 저장
	NPC = Cast<ARACharacterNonPlayer>(ActorInfo->AvatarActor.Get());
	if (!NPC)
	{
		RA_LOG(LogRefrain, Error, TEXT("NPC Not Found"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	const AActor* EventTarget = TriggerEventData ? TriggerEventData->Target.Get() : nullptr;
	TargetPlayer = Cast<ARACharacterPlayer>(const_cast<AActor*>(EventTarget));
	if (!TargetPlayer)
	{
		RA_LOG(LogRefrain, Error, TEXT("TargetPlayer Not Found"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	CombatManager = TargetPlayer->FindComponentByClass<UNPCCombatStateComponent>();
	if (!CombatManager)
	{
		RA_LOG(LogRefrain, Error, TEXT("CombatManager Not Found"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 델리게이트 등록
	UAbilityTask_WaitGameplayEvent* AttackHitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, RefrainGameplayTags::Event_Montage_AttackHit, nullptr, false, true);
	AttackHitTask->EventReceived.AddDynamic(this, &URAGA_NPCCounterableAttack::OnAttackHit);
	AttackHitTask->ReadyForActivation();
	
	// 공격
	Attack();
}

void URAGA_NPCCounterableAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	ClearAttackTiming();
	ClearAttackMotionWarpTarget();
}

void URAGA_NPCCounterableAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URAGA_NPCCounterableAttack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URAGA_NPCCounterableAttack::OnAttackHit(FGameplayEventData Payload)
{
	TryApplyDamageToTargetPlayer();
}

bool URAGA_NPCCounterableAttack::TryApplyDamageToTargetPlayer()
{
	if (!NPC || !TargetPlayer || !DamageEffectClass)
	{
		return false;
	}

	const UAttackHitSweepComponent* AttackHitSweepComponent =
		NPC->FindComponentByClass<UAttackHitSweepComponent>();
	if (!AttackHitSweepComponent)
	{
		RA_LOG(LogRefrain, Warning, TEXT("AttackHitSweepComponent Not Found"));
		return false;
	}

	const TArray<AActor*> HitTargets = AttackHitSweepComponent->HitSweep();
	if (!HitTargets.Contains(TargetPlayer))
	{
		return false;
	}

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetPlayer);
	if (!SourceASC || !TargetASC)
	{
		RA_LOG(LogRefrain, Warning, TEXT("SourceASC or TargetASC Not Found"));
		return false;
	}

	FGameplayEffectSpecHandle DamageSpec = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	if (!DamageSpec.IsValid())
	{
		return false;
	}

	DamageSpec.Data->SetSetByCallerMagnitude(RefrainGameplayTags::Data_Damage, DamageAmount);
	SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpec.Data.Get(), TargetASC);
	RA_LOG(LogRefrain, Log, TEXT("Apply Counterable Attack Damage to %s: %.1f"),
		*GetNameSafe(TargetPlayer), DamageAmount);

	return true;
}

void URAGA_NPCCounterableAttack::UpdateAttackMotionWarpTarget()
{
	const FVector Offset = FVector(100.f, 0.f, 0.f);
	UMotionWarpingComponent* MotionWarpingComponent = NPC->FindComponentByClass<UMotionWarpingComponent>();
	
	// 플레이어
	USkeletalMeshComponent* TargetMesh = TargetPlayer->GetMesh();
	if (!TargetMesh)
	{
		RA_LOG(LogRefrain, Error, TEXT("TargetMesh Not Found"));
		return;
	}
	
	// 모션워핑에 필요한 정보 설정
	MotionWarpingComponent->AddOrUpdateWarpTargetFromComponent(
		FName(TEXT("Player")), TargetMesh, NAME_None, false, 
		EWarpTargetLocationOffsetDirection::VectorFromTargetToOwner, Offset);
}

void URAGA_NPCCounterableAttack::ClearAttackMotionWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComponent = NPC->FindComponentByClass<UMotionWarpingComponent>();
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveAllWarpTargets();
	}
}

void URAGA_NPCCounterableAttack::Attack()
{
	if (!IsValid(AttackMontage))
	{
		RA_LOG(LogRefrain, Error, TEXT("AttackMontage Is Not Valid"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	float TargetBeatTime = CalculatePlayRate(AttackMontage);
	
	// WaitTime이 있을 경우 타이머 설정해서 몽타주 재생
	if (MontageWaitTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(MontageWaitTimerHandle, this, &URAGA_NPCCounterableAttack::PlayAttackMontage, MontageWaitTime, false);
	}
	else
	{
		PlayAttackMontage();
	}
	
	// 위젯 설정
	RhythmTargetWidget = NPC->GetRhythmTargetWidget();
	NPC->SetRhythmWidgetVisibility(true);
	if (!RhythmTargetWidget)
	{
		RA_LOG(LogRefrain, Error, TEXT("RhythmTargetWidget Not Found"));
	}
	else
	{
		// 위젯 업데이트하는 태스크
		URAAT_RhythmTargetWidgetProgress* RhythmTask =
			URAAT_RhythmTargetWidgetProgress::PlayRhythmTargetProgress(
				this,
				RhythmTargetWidget,
				TargetBeatTime
			);
		RhythmTask->ReadyForActivation();
	}
}

void URAGA_NPCCounterableAttack::PlayAttackMontage()
{
	UpdateAttackMotionWarpTarget();
	
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("NPCCounterableAttack"),
			AttackMontage,
			MontagePlayRate,
			NAME_None,
			true,
			1.f,
			MontageStartTime);
	MontageTask->OnCompleted.AddDynamic(this, &URAGA_NPCCounterableAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URAGA_NPCCounterableAttack::OnMontageInterrupted);
	MontageTask->ReadyForActivation();
}

float URAGA_NPCCounterableAttack::CalculatePlayRate(const UAnimMontage* Montage)
{
	// AttackHit 이벤트를 정박 이후(0.1박)에 발동시키는 게 목표
	const FGameplayTag TargetTag = RefrainGameplayTags::Event_Montage_AttackHit;		// 몽타주 안에서 검색할 태그
	const float TargetProgressOnTargetTagNotify = 0.1f;									// 검색한 태그가 위치할 박자 진행도
	const int TargetBeatMultiplier = 2;													// 2박짜리 공격
	const float MinPlayRate = 0.25f;													// 최소 PlayRate
	const float MaxPlayRate = 2.f;														// 최대 PlayRate
	
	// 기본값
	MontagePlayRate = 1.f;
	MontageStartTime = 0.f;
	MontageWaitTime = 0.f;
	
	// 검색한 태그가 위치한 시간
	const float NotifyTime = URAUtils::FindGameplayEventNotifyTime(Montage, TargetTag);
	if (NotifyTime <= 0.f)
	{
		RA_LOG(LogRefrain, Warning, TEXT("Montage Notify Time Not Found: %f"), NotifyTime);
		return -1.f;
	}
	
	UMagicalTimingSubsystem* MagicalTiming = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>();
	if (!MagicalTiming)
	{
		RA_LOG(LogRefrain, Warning, TEXT("MagicalTimingSubsystem Not Found"));
		
		return -1.f;
	}
	if (!MagicalTiming->IsMusicPlaying())
	{
		RA_LOG(LogRefrain, Warning, TEXT("Music Not Playing"));
		return -1.f;
	}
	const float SecondsPerBeat = MagicalTiming->GetSecondsPerBeat(); 
	
	// 목표 박자(정박)까지 시간
	const float TargetBeatTime = MagicalTiming->GetTimeUntilNextBeat(EQuartzCommandQuantization::Beat, TargetBeatMultiplier);
	
	// 계산식...
	const float DesiredNotifyTime = TargetBeatTime + (TargetProgressOnTargetTagNotify * SecondsPerBeat);		// 태그가 발동될 목표 시간
	MontagePlayRate = NotifyTime / DesiredNotifyTime;

	if (MontagePlayRate > MaxPlayRate)
	{
		MontageStartTime = NotifyTime - (MaxPlayRate * DesiredNotifyTime);

		MontagePlayRate = MaxPlayRate;
	}
	else if (MontagePlayRate < MinPlayRate)
	{
		MontageWaitTime = DesiredNotifyTime - (NotifyTime / MinPlayRate);
		
		MontagePlayRate = MinPlayRate;
	}
	
	FQuartzTransportTimeStamp CurrentTimeStamp;
	if (MagicalTiming->GetMusicTimeStamp(CurrentTimeStamp))
	{
		AttackTimeStampBar = CurrentTimeStamp.Bars;
		AttackTimeStampBeat = CurrentTimeStamp.Beat + TargetBeatMultiplier;
		
		const int NumBeats = MagicalTiming->GetMusicData()->NumBeats;
		while (AttackTimeStampBeat > NumBeats)
		{
			AttackTimeStampBeat -= NumBeats;
			AttackTimeStampBar += 1;
		}
		
		SetAttackTiming(AttackTimeStampBar, AttackTimeStampBeat);
	}
	
	RA_LOG(LogRefrain, Log, 
		TEXT("NotifyTime: %.2f, TargetBeatTime: %.2f, DesiredNotifyTime: %.2f, MontagePlayRate: %.2f, MontageStartTime: %.2f, MontageWaitTime: %.2f, AttackTimeStampBar: %d, AttackTimeStampBeat: %d"),
		NotifyTime, TargetBeatTime, DesiredNotifyTime, MontagePlayRate, MontageStartTime, MontageWaitTime, AttackTimeStampBar, AttackTimeStampBeat);

	return TargetBeatTime;
}

void URAGA_NPCCounterableAttack::SetAttackTiming(int32 Bar, int32 Beat)
{
	CombatManager->SetNowCounterableAttackTiming(NPC, Bar, Beat);
}

void URAGA_NPCCounterableAttack::ClearAttackTiming()
{
	if (!CombatManager || !NPC)
	{
		return;
	}
	
	CombatManager->ClearNowCounterableAttackTiming(NPC);
}
