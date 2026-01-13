// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/ConditionalMode/ConditionalMode.h"
#include "WaveSystem/WaveManager.h"

bool UConditionalMode::Init(UWaveExecutionMode* Template, UWaveManager* InWaveManager, UWaveData* InOwningWaveData,
                            const int32 InCurrentWaveIndex)
{
	if (!Super::Init(Template, InWaveManager, InOwningWaveData, InCurrentWaveIndex))
	{
		return false;
	}

	if (const UConditionalMode* ConditionalTemplate = Cast<UConditionalMode>(Template))
	{
		NextWaveIndexTrue = ConditionalTemplate->NextWaveIndexTrue;
		NextWaveIndexFalse = ConditionalTemplate->NextWaveIndexFalse;
		NextWaveCondition = ConditionalTemplate->NextWaveCondition;
	}
	else
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("UConditionalMode::Init - Template is not of type UConditionalMode"));
		return false;
	}

	if (!OwningWaveData->Waves.IsValidIndex(NextWaveIndexTrue) ||
		!OwningWaveData->Waves.IsValidIndex(NextWaveIndexFalse))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("UConditionalMode::Init - NextWaveIndexTrue or NextWaveIndexFalse is out of bounds"));
		return false;
	}

	if (!IsValid(NextWaveCondition))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("UConditionalMode::Init - NextWaveCondition is not valid"));
		return false;
	}
	NextWaveCondition->Init(WaveManager);
	
	return true;
}

void UConditionalMode::ExecuteWave()
{
	//WaveManager->OnWaveStarted.Broadcast(OwningWaveData, CurrentWaveIndex);
	WaveManager->WaveStarted();
}

UWaveData* UConditionalMode::GetNextWaveData()
{
	return OwningWaveData;
}

int32 UConditionalMode::GetNextWaveIndex()
{
	const bool bConditionResult = NextWaveCondition->Evaluate(WaveManager, OwningWaveData, CurrentWaveIndex);
	return bConditionResult ? NextWaveIndexTrue : NextWaveIndexFalse;
}

bool UConditionalMode::HasNextWave()
{
	return NextWaveCondition->HasNextWave(WaveManager, OwningWaveData, CurrentWaveIndex);
}
