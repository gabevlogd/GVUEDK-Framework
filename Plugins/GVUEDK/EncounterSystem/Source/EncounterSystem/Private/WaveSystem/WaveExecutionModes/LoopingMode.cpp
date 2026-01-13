// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/LoopingMode.h"
#include "WaveSystem/WaveManager.h"


int32 ULoopingMode::GetNextWaveIndex()
{
	if (!IsValid(OwningWaveData))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("GetNextWaveIndex called with null WaveData"));
	}
	
	int32 NextIndex = CurrentWaveIndex + 1;
	if (!OwningWaveData->Waves.IsValidIndex(NextIndex))
	{
		NextIndex = 0;
	}
	
	return NextIndex;
}

bool ULoopingMode::HasNextWave()
{
	return true;
}
