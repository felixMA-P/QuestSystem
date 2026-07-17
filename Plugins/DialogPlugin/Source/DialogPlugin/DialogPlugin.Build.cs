// Copyright 2026 Felix Martin Arroyo. All Rights Reserved.

using UnrealBuildTool;

public class DialogPlugin : ModuleRules
{
	public DialogPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[] { "Core" }
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"ApplicationCore",
				"InputCore",
				"DialogPluginRuntime",
				"AssetTools",
				"UnrealEd",
				"PropertyEditor",
				"Projects",
				"ToolMenus",
				"GraphEditor",
				"AppFramework"
			}
		);
	}
}
