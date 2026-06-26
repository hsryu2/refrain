// Copyright Epic Games, Inc. All Rights Reserved.

#include "Modules/ModuleManager.h"

class FRefrainEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		FModuleManager::Get().LoadModulePtr<IModuleInterface>(TEXT("IKRigDeveloper"));
	}
};

IMPLEMENT_MODULE(FRefrainEditorModule, RefrainEditor);
