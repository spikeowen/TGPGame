// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TGP : ModuleRules
{
	public TGP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "AIModule", "Niagara", "GameplayTasks", "NavigationSystem", "OnlineSubsystem", "OnlineSubsystemUtils", "Networking", "Sockets" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });

		if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Linux) || (Target.Platform == UnrealTargetPlatform.Mac))
		{
			PublicDependencyModuleNames.AddRange(new string[] { "Steamworks", "OnlineSubsystemSteam" });
		}
		
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
