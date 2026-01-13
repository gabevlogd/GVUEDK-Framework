// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/PingPongMode.h"

#include "WaveSystem/WaveManager.h"

bool UPingPongMode::bReversing = false;

int32 UPingPongMode::GetNextWaveIndex()
{
	if (!IsValid(OwningWaveData))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("GetNextWaveIndex called with null WaveData"));
	}
	
	int32 NextIndex;
	if (bReversing)
	{
		NextIndex = CurrentWaveIndex - 1;
		if (!OwningWaveData->Waves.IsValidIndex(NextIndex))
		{
			bReversing = false;
			NextIndex = CurrentWaveIndex + 1;
		}
	}
	else
	{
		NextIndex = CurrentWaveIndex + 1;
		if (!OwningWaveData->Waves.IsValidIndex(NextIndex))
		{
			bReversing = true;
			NextIndex = CurrentWaveIndex - 1;
		}
	}
	
	return NextIndex;
}
