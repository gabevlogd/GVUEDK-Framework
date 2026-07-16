// Copyright Villains, Inc. All Rights Reserved.


#include "Subsystems/FMODEventsHandlerSubsystem.h"

#include "FMODEvent.h"
#include "Utility/FMODEventsHandlerUtility.h"

void UFMODEventsHandlerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UFMODEventsHandlerUtility::Initialize(this);
}

void UFMODEventsHandlerSubsystem::BeginDestroy()
{
	Super::BeginDestroy();
	StopAllActiveEvents();
}

bool UFMODEventsHandlerSubsystem::Play2DEvent(UFMODEvent* Event, FFMODEventInstance& OutEvent, const bool bAutoPlay)
{
	if (!Event)
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerSubsystem: Play2DEvent - Event is null."));
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerSubsystem: Play2DEvent - World context is invalid."));
		return false;
	}

	if (World->WorldType != EWorldType::PIE && World->WorldType != EWorldType::Game)
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerSubsystem: Play2DEvent - World context is not valid for playing 2D events."));
		return false;
	}

	OutEvent = UFMODBlueprintStatics::PlayEvent2D(World, Event, bAutoPlay);
	if (!OutEvent.Instance->isValid())
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerSubsystem: Play2DEvent - Failed to play event %s."), *Event->GetName());
		return false;
	}
	
	ActiveEvents.Add(OutEvent);
	return true;
}

bool UFMODEventsHandlerSubsystem::Play3DEvent(UFMODEvent* Event, const FTransform& Transform, FFMODEventInstance& OutEvent, const bool bAutoPlay)
{
	if (!Event)
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerSubsystem: Play3DEvent - Event is null."));
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerSubsystem: Play3DEvent - World context is invalid."));
		return false;
	}

	if (World->WorldType != EWorldType::PIE && World->WorldType != EWorldType::Game)
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerSubsystem: Play3DEvent - World context is not valid for playing 3D events."));
		return false;
	}

	OutEvent = UFMODBlueprintStatics::PlayEventAtLocation(World, Event, Transform, bAutoPlay);
	if (!OutEvent.Instance->isValid())
	{
		UE_LOG(LogFMODEventsHandlerSubsystem, Warning, TEXT("FMODEventsHandlerSubsystem: Play3DEvent - Failed to play event %s."), *Event->GetName());
		return false;
	}
	
	ActiveEvents.Add(OutEvent);
	return true;
}

void UFMODEventsHandlerSubsystem::StopAllActiveEvents()
{
	for (FFMODEventInstance& EventInstance : ActiveEvents)
	{
		if (EventInstance.Instance->isValid())
		{
			EventInstance.Instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		}
	}
	ActiveEvents.Empty();
}
