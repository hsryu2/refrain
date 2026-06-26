// Copyright Epic Games, Inc. All Rights Reserved.

#include "Refrain.h"
#include "Modules/ModuleManager.h"

class FRefrainModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		FDefaultGameModuleImpl::StartupModule();

#if WITH_EDITOR
		FModuleManager::Get().LoadModulePtr<IModuleInterface>(TEXT("IKRigDeveloper"));
#endif
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FRefrainModule, Refrain, "Refrain");

DEFINE_LOG_CATEGORY(LogRefrain);
