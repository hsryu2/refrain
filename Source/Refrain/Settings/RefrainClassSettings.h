// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RefrainClassSettings.generated.h"

/**
 * 서브시스템 등 에디터에서 값을 할당하기 애매한 클래스의 기본값들을 지정해두는 용도의 클래스입니다
 */
UCLASS(Config = Refrain, DefaultConfig, meta = (DisplayName = "Refrain Class Settings"))
class REFRAIN_API URefrainClassSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	virtual FName GetCategoryName() const override;
	
	UPROPERTY(Config, EditAnywhere, Category = "Material")
	TSoftObjectPtr<UMaterialParameterCollection> MagicalTimingMPC;
};
