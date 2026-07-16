#include "AssetTypeActions/FAssetTypeActions_BlueprintTriggerCondition.h"
#include "WorldTriggerSystemEditor.h"
#include "TriggerConditions/BlueprintTriggerCondition.h"

UClass* FAssetTypeActions_BlueprintTriggerCondition::GetSupportedClass() const
{
	return UBlueprintTriggerCondition::StaticClass();
}

FText FAssetTypeActions_BlueprintTriggerCondition::GetName() const
{
	return FText::FromString("Trigger Condition");
}

FColor FAssetTypeActions_BlueprintTriggerCondition::GetTypeColor() const
{
	return FColor(63, 126, 255);
}

uint32 FAssetTypeActions_BlueprintTriggerCondition::GetCategories()
{
	return FWorldTriggerSystemEditorModule::WorldTriggerCategory;
}
