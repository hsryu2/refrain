// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"

void UAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner() || !EventTag.IsValid())
	{
		return;
	}
	
	// 태그에 따라 이벤트를 실행할 예정으로 Payload 생성.
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = MeshComp->GetOwner();
	Payload.Target = MeshComp->GetOwner();
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
	MeshComp->GetOwner(),
	EventTag,
	Payload
	);
	
}
