#include "WorldTriggerSystemEditor.h"

#include "AssetToolsModule.h"

#define LOCTEXT_NAMESPACE "FWorldTriggerSystemEditorModule"

uint32 FWorldTriggerSystemEditorModule::WorldTriggerCategory = 0;

void FWorldTriggerSystemEditorModule::StartupModule()
{
	BlueprintTriggerActionAssetTypeAction = MakeShared<FAssetTypeActions_BlueprintTriggerAction>();
	BlueprintTriggerConditionAssetTypeAction = MakeShared<FAssetTypeActions_BlueprintTriggerCondition>();
	
	IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
	AssetTools.RegisterAssetTypeActions(BlueprintTriggerActionAssetTypeAction.ToSharedRef());
	AssetTools.RegisterAssetTypeActions(BlueprintTriggerConditionAssetTypeAction.ToSharedRef());

	WorldTriggerCategory = AssetTools.RegisterAdvancedAssetCategory(
		FName("WorldTriggerSystem"),
		LOCTEXT("WorldTriggers", "WorldTriggers")
	);
}

void FWorldTriggerSystemEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
		return;

	IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
	AssetTools.UnregisterAssetTypeActions(BlueprintTriggerActionAssetTypeAction.ToSharedRef());
	AssetTools.UnregisterAssetTypeActions(BlueprintTriggerConditionAssetTypeAction.ToSharedRef());
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FWorldTriggerSystemEditorModule, WorldTriggerSystemEditor)