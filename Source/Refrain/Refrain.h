// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// 함수 정보를 문자열로
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)

// 로그 매크로
#define RA_LOG(LogCat, Verbosity, Format, ...) \
	UE_LOG(LogCat, Verbosity, TEXT("[%s %s]"), LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

// 로그 카테고리
DECLARE_LOG_CATEGORY_EXTERN(LogRefrain, Log, All);
