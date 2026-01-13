// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/Factories/WaveCompletionModeFactory.h"
#include "WaveSystem/WaveCompletionModes/Base/WaveCompletionMode.h"

UWaveCompletionMode* UWaveCompletionModeFactory::CreateWaveCompletionMode(UWaveManager* InWaveManager,
                                                                          UWaveData* WaveData, const int32 WaveIndex)
{
	if (!IsValid(WaveData))
	{
		UE_LOG(LogWaveCompletionModeFactory, Error, TEXT("UWaveCompletionModeFactory::CreateWaveCompletionMode called with null WaveData, return nullptr"));
		return nullptr;
	}

	if (!WaveData->Waves.IsValidIndex(WaveIndex))
	{
		UE_LOG(LogWaveCompletionModeFactory, Error, TEXT("UWaveCompletionModeFactory::CreateWaveCompletionMode called with invalid WaveIndex %d, return nullptr"), WaveIndex);
		return nullptr;
	}

	if (!IsValid(InWaveManager))
	{
		UE_LOG(LogWaveCompletionModeFactory, Error, TEXT("UWaveCompletionModeFactory::CreateWaveCompletionMode called with null WaveManager, return nullptr"));
		return nullptr;
	}

	UWaveCompletionMode* Template = WaveData->Waves[WaveIndex].WaveCompletionMode;
	UWaveCompletionMode* NewWaveCompletionMode = NewObject<UWaveCompletionMode>(InWaveManager, Template->GetClass());

	if (NewWaveCompletionMode->Init(Template, InWaveManager))
	{
		return NewWaveCompletionMode;
	}

	UE_LOG(LogWaveCompletionModeFactory, Error, TEXT("UWaveCompletionModeFactory::CreateWaveCompletionMode failed to initialize WaveCompletionMode for wave index %d, return nullptr"), WaveIndex);
	return nullptr;
	
}
