// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Refrain.h"
#include "Abilities/GameplayAbilityTypes.h"

void UAN_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}
	
	if (!EventTag.IsValid())
	{
		RA_LOG(LogRefrain, Warning, TEXT("EventTag Is Not Valid"));
		return;
	}
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!IsValid(ASC))
	{
		return;
	}
	
	// 태그에 따라 이벤트를 실행할 예정으로 Payload 생성.
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = MeshComp->GetOwner();
	Payload.Target = MeshComp->GetOwner();
	
	ASC->HandleGameplayEvent(EventTag, &Payload);
}
