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
		PublicDependencyModuleNames.Add("EditorSubsystem"); // 임시 확인용
		PrivateDependencyModuleNames.AddRange(new[] {
			"UnrealEd", "LevelEditor", "EditorSubsystem", "ToolMenus",
			"AssetRegistry", "InputCore", "Projects", "ContentBrowser"
		});
	}
}
