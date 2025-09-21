// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PUBG : ModuleRules
{
	public PUBG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// 참조 경로 추가 (헤더 파일을 못찾을시 해당 폴더를 참조한다)
		PublicIncludePaths.AddRange(new string[]
		{
			"PUBG",
			"PUBG/Public"
		});

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
            "GameplayTags",
            "GameplayTasks",
            "GameplayAbilities",
            "AnimGraphRuntime",
            "MotionWarping",
            "AIModule",
			"NavigationSystem",
			"NetCore",
			"UMG",
            "ModularGameplay",
            "PhysicsCore"
        });

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"GameplayMessageRuntime",
        });

        //SetupGameplayDebuggerSupport(Target);
        //SetupIrisSupport(Target);

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
