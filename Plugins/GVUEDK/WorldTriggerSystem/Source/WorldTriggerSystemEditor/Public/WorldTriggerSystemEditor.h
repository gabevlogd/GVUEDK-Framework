#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions/FAssetTypeActions_BlueprintTriggerAction.h"
#include "AssetTypeActions/FAssetTypeActions_BlueprintTriggerCondition.h"
#include "Modules/ModuleManager.h"

class FWorldTriggerSystemEditorModule : public IModuleInterface
{
public:
    static uint32 WorldTriggerCategory;
    
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:

    TSharedPtr<FAssetTypeActions_BlueprintTriggerAction> BlueprintTriggerActionAssetTypeAction;
    TSharedPtr<FAssetTypeActions_BlueprintTriggerCondition> BlueprintTriggerConditionAssetTypeAction;
};
