// Fill out your copyright notice in the Description page of Project Settings.


#include "RAUtils.h"

#include "GameplayTagContainer.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimTypes.h"
#include "Animation/AN_SendGameplayEvent.h"

float URAUtils::FindGameplayEventNotifyTime(const UAnimMontage* Montage, const FGameplayTag EventTag)
{
	if (!Montage)
	{
		return -1.f;
	}
	
	for (const FAnimNotifyEvent& NotifyEvent : Montage->Notifies)
	{
		const UAN_SendGameplayEvent* EventNotify = Cast<UAN_SendGameplayEvent>(NotifyEvent.Notify);
		if (EventNotify && EventNotify->EventTag == EventTag)
		{
			return NotifyEvent.GetTime();
		}
	}
	
	return -1.f;
}
