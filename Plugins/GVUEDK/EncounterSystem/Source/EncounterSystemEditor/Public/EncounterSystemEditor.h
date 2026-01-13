#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FEncounterSystemEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:

    TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& Args);

};
