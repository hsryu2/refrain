// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RefrainEditor : ModuleRules
{
	public RefrainEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"IKRigDeveloper"
		});
	}
}
