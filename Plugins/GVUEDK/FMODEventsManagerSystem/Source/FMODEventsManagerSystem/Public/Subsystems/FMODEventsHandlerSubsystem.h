// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODBlueprintStatics.h"
#include "Subsystems/WorldSubsystem.h"
#include "FMODEventsHandlerSubsystem.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogFMODEventsHandlerSubsystem, All, All);

/**
 * 
 */
UCLASS()
class FMODEVENTSMANAGERSYSTEM_API UFMODEventsHandlerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void BeginDestroy() override;

	bool Play2DEvent(UFMODEvent* Event, FFMODEventInstance& OutEvent, const bool bAutoPlay = true);

	bool Play3DEvent(UFMODEvent* Event, const FTransform& Transform, FFMODEventInstance& OutEvent,
	                 const bool bAutoPlay = true);

	void StopAllActiveEvents();

private:
	friend class UFMODEventsHandlerUtility;

	UPROPERTY()
	TArray<FFMODEventInstance> ActiveEvents;
};
