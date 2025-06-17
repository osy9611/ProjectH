// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ModularGame : ModuleRules
{
    public ModularGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "ModularGameplay",
                "InputCore",
                "Engine",
                "Slate",
                "SlateCore",
                "UMG",
                "CommonUI",
                //GAS
			    "GameplayTags",
                "GameplayTasks",
                "GameplayAbilities",
                //Niagara
                "Niagara",
                "NiagaraCore",
			    //MetaSound
			    "AudioMixer",
                "AudioModulation",
                "MetasoundEngine"
            }
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
            }
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );
    }
}
