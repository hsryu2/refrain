// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ComboCheck.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"

void UAnimNotify_ComboCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner() || !EventTag.IsValid())
	{
		return;
	}
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
