// Copyright Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class PresentationFinal : ModuleRules
{
	public PresentationFinal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "Slate","SlateCore", "WhiteBoard", "GameLiftServerSDK", "OVRLipSync"});

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "PhysXVehicle/Public"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "3DGraphPlots/Public"));
	}
}
