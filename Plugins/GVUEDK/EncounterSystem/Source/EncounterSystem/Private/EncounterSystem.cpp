// Copyright Epic Games, Inc. All Rights Reserved.

#include "EncounterSystem.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Settings/EncounterSystemSettings.h"

#define LOCTEXT_NAMESPACE "FEncounterSystemModule"

void FEncounterSystemModule::StartupModule()
{
	if (ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingModule->RegisterSettings("Project", "GVUEDK", "Encounter System",
			LOCTEXT("RuntimeSettingsName", "Encounter System"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Encounter System settings"),
			GetMutableDefault<UEncounterSystemSettings>()
		);
	}
}

void FEncounterSystemModule::ShutdownModule()
{
	if (ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingModule->UnregisterSettings("Project", "GVUEDK", "Encounter System");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEncounterSystemModule, EncounterSystem)