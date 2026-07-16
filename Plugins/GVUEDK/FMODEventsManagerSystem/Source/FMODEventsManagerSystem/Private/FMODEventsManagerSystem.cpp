// Copyright Epic Games, Inc. All Rights Reserved.

#include "FMODEventsManagerSystem.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Settings/FMODEventsSettings.h"

#define LOCTEXT_NAMESPACE "FFMODEventsManagerSystemModule"

void FFMODEventsManagerSystemModule::StartupModule()
{
	if (ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingModule->RegisterSettings("Project", "GVUEDK", "FMOD Events Manager System",
			LOCTEXT("RuntimeSettingsName", "FMOD Events Manager System"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the registered FMOD Events"),
			GetMutableDefault<UFMODEventsSettings>()
		);

	}
}

void FFMODEventsManagerSystemModule::ShutdownModule()
{
	if (ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingModule->UnregisterSettings("Project", "GVUEDK", "FMOD Events Manager System");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFMODEventsManagerSystemModule, FMODEventsManagerSystem)