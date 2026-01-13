using UnrealBuildTool;

public class EncounterSystemEditor : ModuleRules
{
    public EncounterSystemEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",          // NECESSARIO → FEditorModeInfo, FEdMode, FEditorModeRegistry
                "SlateCore",
                "Slate",
                "InputCore",
                "ToolMenus",
                "PropertyEditor"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "EncounterSystem",
                "EditorFramework"
            }
        );
    }
}