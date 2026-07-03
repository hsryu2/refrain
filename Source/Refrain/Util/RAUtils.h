// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RAUtils.generated.h"

class UAnimMontage;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class REFRAIN_API URAUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// 몽타주 안에서 해당 몽타주 안의 UAN_SendGameplayEvent 노티파이가 위치한 시간을 반환하는 함수. 실패 시 -1.f 반환
	UFUNCTION(BlueprintCallable, Category = "Refrain|Util")
	static float FindGameplayEventNotifyTime(const UAnimMontage* Montage, const FGameplayTag EventTag);

};
