// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/RACharacterNonPlayer.h"
#include "EngineUtils.h"
#include "GA/Attribute/RAAttributeSet.h"
#include "HAL/IConsoleManager.h"
#include "Refrain.h"

#if !UE_BUILD_SHIPPING
static FAutoConsoleCommandWithWorldAndArgs CVarSetNPCHealth(
	TEXT("ra.SetNPCHealth"),
	TEXT("Sets Health for all NPCs in the current world. Usage: ra.SetNPCHealth 500"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (!World)
			{
				UE_LOG(LogRefrain, Warning, TEXT("ra.SetNPCHealth: World is invalid."));
				return;
			}

			if (Args.Num() < 1)
			{
				UE_LOG(LogRefrain, Warning, TEXT("Usage: ra.SetNPCHealth <Health>"));
				return;
			}

			float NewHealth = 0.0f;
			if (!LexTryParseString(NewHealth, *Args[0]))
			{
				UE_LOG(LogRefrain, Warning, TEXT("Usage: ra.SetNPCHealth <Health>"));
				return;
			}

			int32 ChangedCount = 0;

			for (TActorIterator<ARACharacterNonPlayer> It(World); It; ++It)
			{
				ARACharacterNonPlayer* NPC = *It;
				if (!IsValid(NPC))
				{
					continue;
				}

				UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(NPC);
				if (!ASC)
				{
					continue;
				}

				if (NewHealth > ASC->GetNumericAttribute(URAAttributeSet::GetMaxHealthAttribute()))
				{
					ASC->SetNumericAttributeBase(URAAttributeSet::GetMaxHealthAttribute(), NewHealth);
				}

				ASC->SetNumericAttributeBase(URAAttributeSet::GetHealthAttribute(), NewHealth);
				++ChangedCount;
			}

			UE_LOG(LogRefrain, Log, TEXT("ra.SetNPCHealth: Set %d NPC(s) Health to %.1f."), ChangedCount, NewHealth);
		}
	)
);
#endif
