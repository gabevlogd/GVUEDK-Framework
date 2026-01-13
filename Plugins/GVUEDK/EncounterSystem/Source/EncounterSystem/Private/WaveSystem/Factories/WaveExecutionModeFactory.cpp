

#include "WaveSystem/Factories/WaveExecutionModeFactory.h"
#include "WaveSystem/WaveManager.h"
#include "WaveSystem/WaveExecutionModes/Base/WaveExecutionMode.h"

UWaveExecutionMode* UWaveExecutionModeFactory::CreateWaveExecutionMode(UWaveManager* InWaveManager, UWaveData* Data,
                                                                       const int32 WaveIndex)
{
	if (!IsValid(Data))
	{
		UE_LOG(LogWaveExecutionModeFactory, Error, TEXT("UWaveExecutionModeFactory::CreateWaveExecutionMode called with null WaveData, return nullptr"));
		return nullptr;
	}

	if (!Data->Waves.IsValidIndex(WaveIndex))
	{
		UE_LOG(LogWaveExecutionModeFactory, Error, TEXT("UWaveExecutionModeFactory::CreateWaveExecutionMode called with invalid WaveIndex %d, return nullptr"), WaveIndex);
		return nullptr;
	}

	if (!IsValid(InWaveManager))
	{
		UE_LOG(LogWaveExecutionModeFactory, Error, TEXT("UWaveExecutionModeFactory::CreateWaveExecutionMode called with null WaveManager, return nullptr"));
		return nullptr;
	}

	UWaveExecutionMode* Template = Data->Waves[WaveIndex].WaveExecutionMode;
	UWaveExecutionMode* NewWaveExecutionMode = NewObject<UWaveExecutionMode>(InWaveManager, Template->GetClass());

	if (NewWaveExecutionMode->Init(Template, InWaveManager, Data, WaveIndex))
	{
		return NewWaveExecutionMode;
	}
	
	UE_LOG(LogWaveExecutionModeFactory, Error, TEXT("UWaveExecutionModeFactory::CreateWaveExecutionMode failed to initialize WaveExecutionMode for wave index %d, return nullptr"), WaveIndex);
	return nullptr;
}
