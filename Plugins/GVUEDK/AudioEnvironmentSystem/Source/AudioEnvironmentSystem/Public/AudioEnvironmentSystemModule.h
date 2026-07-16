#pragma once

#include "Modules/ModuleManager.h"

/** Dedicated log category for the Audio Environment System. */
AUDIOENVIRONMENTSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(LogAudioEnvironment, Log, All);

class FAudioEnvironmentSystemModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
