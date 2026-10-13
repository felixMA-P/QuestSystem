// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

using UnrealBuildTool;

public class NPCManagerSystem : ModuleRules
{
	public NPCManagerSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[] { "Core", "CoreUObject", "Engine", "GameplayTags", "DeveloperSettings" }
		);
	}
}
