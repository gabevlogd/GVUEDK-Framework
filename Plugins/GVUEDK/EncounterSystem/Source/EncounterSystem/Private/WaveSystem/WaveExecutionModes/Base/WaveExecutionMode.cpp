// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/Base/WaveExecutionMode.h"

#include "WaveSystem/WaveManager.h"

bool UWaveExecutionMode::Init(UWaveExecutionMode* Template, UWaveManager* InWaveManager, UWaveData* InOwningWaveData,
                              const int32 InCurrentWaveIndex)
{
	if (!IsValid(InWaveManager))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("UWaveExecutionMode::Init - InWaveManager is not valid"));
		return false;
	}

	if (!IsValid(InOwningWaveData))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("UWaveExecutionMode::Init - InOwningWaveData is not valid"));
		return false;
	}

	WaveManager = InWaveManager;
	OwningWaveData = InOwningWaveData;
	CurrentWaveIndex = InCurrentWaveIndex;
	bAutoStartNext = Template->bAutoStartNext;
	DelayBeforeNextWaveRange = Template->DelayBeforeNextWaveRange;
	return true;
}

float UWaveExecutionMode::GetDelayBeforeNextWave() const
{
	float MinRange = 0.f;
	float MaxRange = 0.f;

	if (DelayBeforeNextWaveRange.HasLowerBound() && DelayBeforeNextWaveRange.GetLowerBoundValue() > 0.f)
	{
		MinRange = DelayBeforeNextWaveRange.GetLowerBoundValue();
	}

	if (DelayBeforeNextWaveRange.HasUpperBound() && DelayBeforeNextWaveRange.GetUpperBoundValue() > 0.f)
	{
		MaxRange = DelayBeforeNextWaveRange.GetUpperBoundValue();
	}

	if (MaxRange <= KINDA_SMALL_NUMBER && MinRange <= KINDA_SMALL_NUMBER)
	{
		return -1.f;
	}
	
	return FMath::FRandRange(MinRange, MaxRange);
}
