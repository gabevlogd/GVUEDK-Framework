// Copyright (c) 2024. All Rights Reserved.

using UnrealBuildTool;

public class AudioEnvironmentSystem : ModuleRules
{
    public AudioEnvironmentSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "DeveloperSettings",   // UDeveloperSettings (AudioEnvironmentSettings)
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            // Intentionally minimal.
            // Audio middleware modules (FMOD, Wwise, etc.) are added by extension plugins,
            // NOT by this core system.
        });
    }
}
