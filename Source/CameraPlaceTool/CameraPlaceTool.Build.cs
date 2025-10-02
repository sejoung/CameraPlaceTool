// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CameraPlaceTool : ModuleRules
{
	public CameraPlaceTool(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new[] {
			"Core", "CoreUObject", "Engine", "Slate", "SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "Slate", "SlateCore",
			"UnrealEd", "LevelEditor", "EditorSubsystem", "ToolMenus",
			"AssetRegistry", "InputCore", "Projects", "ContentBrowser",
			"Blutility"
		});
	}
}
