// Copyright Epic Games, Inc. All Rights Reserved.

#include "PopUpSystem.h"

#include "GameplayTagsManager.h"
#include "ISettingsModule.h"
#include "PopUpSystemSettings.h"

#define LOCTEXT_NAMESPACE "FPopUpSystemModule"

void FPopUpSystemModule::StartupModule()
{
	if (ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingModule->RegisterSettings("Project", "GVUEDK", "Pop Up System",
			LOCTEXT("RuntimeSettingsName", "Pop Up System"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the registered Pop Up"),
			GetMutableDefault<UPopUpSystemSettings>()
		);
	}

	FName TagName = FName("PopUpWidget");
	if (!UGameplayTagsManager::Get().RequestGameplayTag(TagName, false).IsValid())
	{
		UGameplayTagsManager::Get().AddNativeGameplayTag(TagName, TEXT("Base category for PopUpWidget"));
	}
}

void FPopUpSystemModule::ShutdownModule()
{
	if (ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingModule->UnregisterSettings("Project", "GVUEDK", "Pop Up System");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPopUpSystemModule, PopUpSystem)