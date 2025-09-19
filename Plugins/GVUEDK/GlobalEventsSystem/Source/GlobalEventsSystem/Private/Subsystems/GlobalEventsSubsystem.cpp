// Copyright Villains, Inc. All Rights Reserved.


#include "Subsystems/GlobalEventsSubsystem.h"
#include "GameplayTagContainer.h"
#include "Utility/GlobalEventsUtility.h"

void UGlobalEventsSubsystem::BindEvent(const FGameplayTag& EventTag, const FGloablEvent& Event)
{
	if (!EventTag.IsValid())
	{
		UE_LOG(LogGlobalEventsSystem, Warning, TEXT("Trying bind event but EventTag is invalid"));
		return;
	}

	EventMap.FindOrAdd(EventTag, FGlobalEventSignature()).AddUnique(Event);
}

void UGlobalEventsSubsystem::BindObjectEvent(const FGameplayTag& EventTag, const FGlobalEventWithObject& Event)
{
	if (!EventTag.IsValid())
	{
		UE_LOG(LogGlobalEventsSystem, Warning, TEXT("Trying bind event but EventTag is invalid"));
		return;
	}

	ObjectEventMap.FindOrAdd(EventTag, FGlobalEventSignatureWithObject()).AddUnique(Event);
}

void UGlobalEventsSubsystem::UnbindEvent(const FGameplayTag& EventTag, const FGloablEvent& Event)
{
	if (!EventTag.IsValid())
	{
		UE_LOG(LogGlobalEventsSystem, Warning, TEXT("Trying unbind event but EventTag is invalid"));
		return;
	}

	if (EventMap.Contains(EventTag))
	{
		EventMap[EventTag].Remove(Event);
	}
	else
	{
		UE_LOG(LogGlobalEventsSystem, Warning, TEXT("Trying unbind event but EventTag is not found"));
	}
}

void UGlobalEventsSubsystem::UnbindObjectEvent(const FGameplayTag& EventTag, const FGlobalEventWithObject& Event)
{
	if (!EventTag.IsValid())
	{
		UE_LOG(LogGlobalEventsSystem, Warning, TEXT("Trying unbind event but EventTag is invalid"));
		return;
	}

	if (ObjectEventMap.Contains(EventTag))
	{
		ObjectEventMap[EventTag].Remove(Event);
	}
	else
	{
		UE_LOG(LogGlobalEventsSystem, Warning, TEXT("Trying unbind event but EventTag is not found"));
	}
}

void UGlobalEventsSubsystem::CallEvent(const FGameplayTag& EventTag)
{
	if (!EventTag.IsValid())
	{
		UE_LOG(LogGlobalEventsSystem, Warning, TEXT("Trying call event but EventTag is invalid"));
		return;
	}

	if (EventMap.Contains(EventTag))
	{
		EventMap[EventTag].Broadcast();
	}
	else
	{
		UE_LOG(LogGlobalEventsSystem, Warning, TEXT("Trying call event but EventTag is not found"));
	}
}

void UGlobalEventsSubsystem::CallObjectEvent(const FGameplayTag& EventTag, UObject* Object)
{
	if (!EventTag.IsValid())
	{
		UE_LOG(LogGlobalEventsSystem, Warning, TEXT("Trying call event but EventTag is invalid"));
		return;
	}

	if (ObjectEventMap.Contains(EventTag))
	{
		ObjectEventMap[EventTag].Broadcast(Object);
	}
	else
	{
		UE_LOG(LogGlobalEventsSystem, Warning, TEXT("Trying call event but EventTag is not found"));
	}
}

void UGlobalEventsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UGlobalEventsUtility::Initialize(this);
}
