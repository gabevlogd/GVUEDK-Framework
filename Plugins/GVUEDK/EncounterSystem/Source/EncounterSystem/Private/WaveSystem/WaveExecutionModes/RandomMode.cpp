// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/RandomMode.h"

bool URandomMode::Init(UWaveExecutionMode* Template, UWaveManager* InWaveManager, UWaveData* InOwningWaveData,
	const int32 InCurrentWaveIndex)
{
	if (Super::Init(Template, InWaveManager, InOwningWaveData, InCurrentWaveIndex))
	{
		MaxRandomIndex = InOwningWaveData->Waves.Num() - 1;
		return true;
	}
	return false;
}

int32 URandomMode::GetNextWaveIndex()
{
	return GetRandomIndex();
}

int32 URandomMode::GetRandomIndex() const
{
	return FMath::RandRange(0, MaxRandomIndex);
}
