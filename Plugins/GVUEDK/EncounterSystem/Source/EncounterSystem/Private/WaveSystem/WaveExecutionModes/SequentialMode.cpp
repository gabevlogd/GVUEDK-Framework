// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/SequentialMode.h"
#include "WaveSystem/WaveManager.h"


void USequentialMode::ExecuteWave()
{
	//WaveManager->OnWaveStarted.Broadcast(OwningWaveData, CurrentWaveIndex);
	WaveManager->WaveStarted();
}

UWaveData* USequentialMode::GetNextWaveData()
{
	return OwningWaveData;
}

int32 USequentialMode::GetNextWaveIndex()
{
	return CurrentWaveIndex + 1;
}

bool USequentialMode::HasNextWave()
{
	return OwningWaveData->Waves.IsValidIndex(CurrentWaveIndex + 1);
}
