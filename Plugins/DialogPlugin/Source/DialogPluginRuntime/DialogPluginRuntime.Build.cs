// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

using UnrealBuildTool;

public class DialogPluginRuntime : ModuleRules
{
	public DialogPluginRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[] { "Core", "CoreUObject", "Engine", "GameplayTags", "UMG" }
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
			}
		);
	}
}
