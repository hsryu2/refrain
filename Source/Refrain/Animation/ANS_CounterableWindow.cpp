// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_CounterableWindow.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "RefrainGameplayTags.h"
#include "Components/SkeletalMeshComponent.h"

void UANS_CounterableWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!IsValid(ASC))
	{
		return;
	}
	
	ASC->AddLooseGameplayTag(RefrainGameplayTags::State_Attacking_Counterable_InWindow);
}

void UANS_CounterableWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!IsValid(ASC))
	{
		return;
	}
	
	ASC->RemoveLooseGameplayTag(RefrainGameplayTags::State_Attacking_Counterable_InWindow);
}
