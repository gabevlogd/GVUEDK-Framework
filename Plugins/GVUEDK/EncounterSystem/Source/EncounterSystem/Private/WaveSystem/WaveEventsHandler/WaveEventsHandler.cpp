// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveEventsHandler/WaveEventsHandler.h"

#include "WaveSystem/WaveManager.h"

void UWaveEventsHandler::Init(UWaveManager* InWaveManager, UWaveData* InWaveData, const int32 InWaveIndex)
{
	if (!IsValid(InWaveManager))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("WaveEventsHandler Init called with null WaveManager"));
		return;
	}

	if (!IsValid(InWaveData))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("WaveEventsHandler Init called with null WaveData"));
		return;
	}
	
	WaveManager = InWaveManager;
	WaveData = InWaveData;
	WaveIndex = InWaveIndex;
	World = InWaveManager->GetWorld();

	OnInitialize(InWaveManager, InWaveData, InWaveIndex);
}
