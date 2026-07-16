// Copyright Villains, Inc. All Rights Reserved.


#include "Utility/FMODEventsHandlerUtility.h"

TWeakObjectPtr<UFMODEventsHandlerSubsystem> UFMODEventsHandlerUtility::EventsHandlerSubsystem = nullptr;

void UFMODEventsHandlerUtility::Initialize(UFMODEventsHandlerSubsystem* InEventsHandlerSubsystem)
{
	EventsHandlerSubsystem = InEventsHandlerSubsystem;
}

bool UFMODEventsHandlerUtility::Play2DEvent(UFMODEvent* Event, FFMODEventInstance& OutEvent, bool bAutoPlay)
{
	if (!Initialized())
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerUtility: Play2DEvent - EventsHandlerSubsystem is not initialized."));
		return false;
	}

	UFMODEventsHandlerSubsystem* Subsystem = EventsHandlerSubsystem.Get();
	if (!IsValid(Subsystem))
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerUtility: Play2DEvent - EventsHandlerSubsystem is not valid."));
		return false;
	}

	return Subsystem->Play2DEvent(Event, OutEvent, bAutoPlay);
}

bool UFMODEventsHandlerUtility::Play3DEvent(UFMODEvent* Event, const FTransform& Transform,
	FFMODEventInstance& OutEvent, const bool bAutoPlay)
{
	if (!Initialized())
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerUtility: Play2DEvent - EventsHandlerSubsystem is not initialized."));
		return false;
	}

	UFMODEventsHandlerSubsystem* Subsystem = EventsHandlerSubsystem.Get();
	if (!IsValid(Subsystem))
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerUtility: Play3DEvent - EventsHandlerSubsystem is not valid."));
		return false;
	}

	return EventsHandlerSubsystem->Play3DEvent(Event, Transform, OutEvent, bAutoPlay);
}


