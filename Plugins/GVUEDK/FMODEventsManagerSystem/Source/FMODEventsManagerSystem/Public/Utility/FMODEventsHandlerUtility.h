// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystems/FMODEventsHandlerSubsystem.h"
#include "FMODEventsHandlerUtility.generated.h"

/**
 * 
 */
UCLASS()
class FMODEVENTSMANAGERSYSTEM_API UFMODEventsHandlerUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static void Initialize(UFMODEventsHandlerSubsystem* InEventsHandlerSubsystem);
	
	static void Deinitialize() { EventsHandlerSubsystem = nullptr; }

	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = "bAutoPlay"))
	static bool Play2DEvent (UFMODEvent* Event, FFMODEventInstance& OutEvent, const bool bAutoPlay = true);

	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = "bAutoPlay"))
	static bool Play3DEvent(UFMODEvent* Event, const FTransform& Transform, FFMODEventInstance& OutEvent, const bool bAutoPlay = true);

private:

	static bool Initialized() { return EventsHandlerSubsystem.IsValid(); }

private:
	
	static TWeakObjectPtr<UFMODEventsHandlerSubsystem> EventsHandlerSubsystem;
};
