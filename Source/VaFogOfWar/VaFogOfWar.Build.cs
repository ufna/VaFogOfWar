// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

using UnrealBuildTool;

public class VaFogOfWar : ModuleRules
{
    public VaFogOfWar(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RenderCore",
                "RHI"
            }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine"
            }
            );

        PublicDefinitions.Add("WITH_VA_FOGOFWAR=1");
    }
}
