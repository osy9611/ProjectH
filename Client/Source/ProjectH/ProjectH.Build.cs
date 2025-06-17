// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectH : ModuleRules
{
	public ProjectH(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{
            "Core",
            "CoreUObject",
            "Engine", 
			//GAS
			"GameplayTags",
            "GameplayTasks",
            "GameplayAbilities",
			//Game Feature
			"ModularGameplay",
            "GameFeatures",
			//Input
            "InputCore",
            "EnhancedInput",
			//CommonUI
			"CommonInput",
            "CommonUI",
			//CommonGame
            "CommonGame",
			// UMG
            "UMG",
			// Slate
            "Slate",
            "SlateCore",
			//AI
			"AIModule",
			//GameplayMessageRuntime
            "GameplayMessageRuntime",
			//Niagara
            "Niagara",
            "NiagaraCore",
			//MetaSound
			"AudioMixer",
            "AudioModulation",
            "MetasoundEngine",
			//ModularGame
			"ModularGame",
			//Paper2D
			"PaperZD",
            "Paper2D",

			//Level Sequence
			"MovieScene", 
			"MovieSceneTracks", 
			"LevelSequence"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
