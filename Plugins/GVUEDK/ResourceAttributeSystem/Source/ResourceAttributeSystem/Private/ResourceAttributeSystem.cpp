// Copyright Epic Games, Inc. All Rights Reserved.

#include "ResourceAttributeSystem.h"

#include "GameplayTagsManager.h"

#define LOCTEXT_NAMESPACE "FResourceAttributeSystemModule"

void FResourceAttributeSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FName TagName = FName("ResourceAttribute.Health");
	if (!UGameplayTagsManager::Get().RequestGameplayTag(TagName, false).IsValid())
	{
		UGameplayTagsManager::Get().AddNativeGameplayTag(TagName, TEXT("Resource Attribute Tag for Health Attribute"));
	}

	TagName = FName("ResourceAttribute.Stamina");
	if (!UGameplayTagsManager::Get().RequestGameplayTag(TagName, false).IsValid())
	{
		UGameplayTagsManager::Get().AddNativeGameplayTag(TagName, TEXT("Resource Attribute Tag for Stamina Attribute"));
	}
}

void FResourceAttributeSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FResourceAttributeSystemModule, ResourceAttributeSystem)