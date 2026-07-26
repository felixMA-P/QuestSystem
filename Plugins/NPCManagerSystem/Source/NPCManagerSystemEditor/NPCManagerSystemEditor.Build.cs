// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NPCManagerSystemEditor : ModuleRules
{
	public NPCManagerSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"GameplayTags",
				"BlueprintGraph",
				"KismetCompiler",
				"UnrealEd",
				"GraphEditor",
				"NPCManagerSystem",
			}
			);
	}
}
