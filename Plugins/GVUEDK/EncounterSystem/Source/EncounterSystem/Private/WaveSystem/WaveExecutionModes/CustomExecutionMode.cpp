// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/CustomExecutionMode.h"

#include "WaveSystem/WaveManager.h"

bool UCustomExecutionMode::Init(UWaveExecutionMode* Template, UWaveManager* InWaveManager, UWaveData* InOwningWaveData,
	const int32 InCurrentWaveIndex)
{
	if (Super::Init(Template, InWaveManager, InOwningWaveData, InCurrentWaveIndex))
	{
		World = InWaveManager->GetWorld();
		return Init(Template, InWaveManager, InOwningWaveData, InCurrentWaveIndex);
	}
	
	return false;
}

void UCustomExecutionMode::ExecuteWave()
{
	ExecuteWave(WaveManager, OwningWaveData, CurrentWaveIndex);
}

UWaveData* UCustomExecutionMode::GetNextWaveData()
{
	return GetNextWaveData(WaveManager, OwningWaveData, CurrentWaveIndex);
}

int32 UCustomExecutionMode::GetNextWaveIndex()
{
	return GetNextWaveIndex(WaveManager, OwningWaveData, CurrentWaveIndex);
}

bool UCustomExecutionMode::HasNextWave()
{
	return HasNextWave(WaveManager, OwningWaveData, CurrentWaveIndex);
}

void UCustomExecutionMode::ExecuteWave_Implementation(const UWaveManager* InWaveManager,
                                                      const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex)
{
	//InWaveManager->OnWaveStarted.Broadcast(InOwningWaveData, InCurrentWaveIndex);
	WaveManager->WaveStarted();
}

UWaveData* UCustomExecutionMode::GetNextWaveData_Implementation(const UWaveManager* InWaveManager,
	const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex)
{
	return const_cast<UWaveData*>(InOwningWaveData);
}

int32 UCustomExecutionMode::GetNextWaveIndex_Implementation(const UWaveManager* InWaveManager,
	const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex)
{
	return InCurrentWaveIndex + 1;
}

bool UCustomExecutionMode::HasNextWave_Implementation(const UWaveManager* InWaveManager,
	const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex)
{
	const int32 NextWaveIndex = GetNextWaveIndex(WaveManager, OwningWaveData, CurrentWaveIndex);
	return InOwningWaveData->Waves.IsValidIndex(NextWaveIndex);
}
