// Copyright Villains, Inc. All Rights Reserved.


#include "Utility/GlobalEventsUtility.h"
#include "Subsystems/GlobalEventsSubsystem.h"

UGlobalEventsSubsystem* UGlobalEventsUtility::GlobalEventsSubsystem = nullptr;

void UGlobalEventsUtility::Initialize(UGlobalEventsSubsystem* GlobalEvents)
{
	if (!IsValid(GlobalEvents))
	{
		UE_LOG(LogGlobalEventsSystem, Error, TEXT("GlobalEventsUtility initialization failed. GlobalEventsSubsystem is invalid."));
		return;
	}

	GlobalEventsSubsystem = GlobalEvents;
}

void UGlobalEventsUtility::BindEvent(const FGameplayTag EventTag, const FGloablEvent& Event)
{
	if (!IsValid(GlobalEventsSubsystem))
	{
		UE_LOG(LogGlobalEventsSystem, Error, TEXT("GlobalEventsUtility is not initialized. GlobalEventsSubsystem is invalid."));
		return;
	}

	GlobalEventsSubsystem->BindEvent(EventTag, Event);
}

void UGlobalEventsUtility::BindObjectEvent(const FGameplayTag EventTag, const FGlobalEventWithObject& Event)
{
	if (!IsValid(GlobalEventsSubsystem))
	{
		UE_LOG(LogGlobalEventsSystem, Error, TEXT("GlobalEventsUtility is not initialized. GlobalEventsSubsystem is invalid."));
		return;
	}

	GlobalEventsSubsystem->BindObjectEvent(EventTag, Event);
}

void UGlobalEventsUtility::UnbindEvent(const FGameplayTag EventTag, const FGloablEvent& Event)
{
	if (!IsValid(GlobalEventsSubsystem))
	{
		UE_LOG(LogGlobalEventsSystem, Error, TEXT("GlobalEventsUtility is not initialized. GlobalEventsSubsystem is invalid."));
		return;
	}

	GlobalEventsSubsystem->UnbindEvent(EventTag, Event);
}

void UGlobalEventsUtility::UnbindObjectEvent(const FGameplayTag EventTag, const FGlobalEventWithObject& Event)
{
	if (!IsValid(GlobalEventsSubsystem))
	{
		UE_LOG(LogGlobalEventsSystem, Error, TEXT("GlobalEventsUtility is not initialized. GlobalEventsSubsystem is invalid."));
		return;
	}

	GlobalEventsSubsystem->UnbindObjectEvent(EventTag, Event);
}

void UGlobalEventsUtility::CallEvent(const FGameplayTag EventTag)
{
	if (!IsValid(GlobalEventsSubsystem))
	{
		UE_LOG(LogGlobalEventsSystem, Error, TEXT("GlobalEventsUtility is not initialized. GlobalEventsSubsystem is invalid."));
		return;
	}

	GlobalEventsSubsystem->CallEvent(EventTag);
}

void UGlobalEventsUtility::CallObjectEvent(const FGameplayTag EventTag, UObject* Object)
{
	if (!IsValid(GlobalEventsSubsystem))
	{
		UE_LOG(LogGlobalEventsSystem, Error, TEXT("GlobalEventsUtility is not initialized. GlobalEventsSubsystem is invalid."));
		return;
	}

	GlobalEventsSubsystem->CallObjectEvent(EventTag, Object);
}
