#include "AssetTypeActions/FAssetTypeActions_BlueprintTriggerAction.h"
#include "WorldTriggerSystemEditor.h"
#include "TriggerActions/BlueprintTriggerAction.h"

UClass* FAssetTypeActions_BlueprintTriggerAction::GetSupportedClass() const
{
	return UBlueprintTriggerAction::StaticClass();
}

FText FAssetTypeActions_BlueprintTriggerAction::GetName() const
{
	return FText::FromString("Trigger Action");
}

FColor FAssetTypeActions_BlueprintTriggerAction::GetTypeColor() const
{
	return FColor(63, 126, 255);
}

uint32 FAssetTypeActions_BlueprintTriggerAction::GetCategories()
{
	return FWorldTriggerSystemEditorModule::WorldTriggerCategory;
}
