// Copyright Epic Games, Inc. All Rights Reserved.

#include "SequencesManagerSystem.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Settings/SequencesManagerSettings.h"

#define LOCTEXT_NAMESPACE "FSequencesManagerSystemModule"

void FSequencesManagerSystemModule::StartupModule()
{
	if (ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingModule->RegisterSettings("Project", "GVUEDK", "Sequences Manager Settings",
			LOCTEXT("RuntimeSettingsName", "Sequences Manager Settings"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Sequences Manager Settings"),
			GetMutableDefault<USequencesManagerSettings>()
		);
	}
}

void FSequencesManagerSystemModule::ShutdownModule()
{
	if (ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingModule->UnregisterSettings("Project", "GVUEDK", "Sequences Manager Settings");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSequencesManagerSystemModule, SequencesManagerSystem)