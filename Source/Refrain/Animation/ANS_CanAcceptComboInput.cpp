// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ANS_CanAcceptComboInput.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "RefrainGameplayTags.h"

void UANS_CanAcceptComboInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
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
	
	const FGameplayTag ComboInputStartTag = RefrainGameplayTags::Event_Montage_ComboStart;
	
	// 태그에 따라 이벤트를 실행할 예정으로 Payload 생성.
	FGameplayEventData Payload;
	Payload.EventTag = ComboInputStartTag;
	Payload.Instigator = MeshComp->GetOwner();
	Payload.Target = MeshComp->GetOwner();
	
	ASC->HandleGameplayEvent(ComboInputStartTag, &Payload);
}

void UANS_CanAcceptComboInput::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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
	
	const FGameplayTag ComboInputEndTag = RefrainGameplayTags::Event_Montage_ComboEnd;
	
	// 태그에 따라 이벤트를 실행할 예정으로 Payload 생성.
	FGameplayEventData Payload;
	Payload.EventTag = ComboInputEndTag;
	Payload.Instigator = MeshComp->GetOwner();
	Payload.Target = MeshComp->GetOwner();
	
	ASC->HandleGameplayEvent(ComboInputEndTag, &Payload);
}
