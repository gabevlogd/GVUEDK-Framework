// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveCompletionModes/KillAllMode.h"
#include "WaveSystem/WaveManager.h"

bool UKillAllMode::Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager)
{
	WaveManager = InWaveManager;
	bCallCompletionCheckOnEnemyDeath = true;

	if (const UKillAllMode* KillAllTemplate = Cast<UKillAllMode>(Template))
	{
		bGlobalKill = KillAllTemplate->bGlobalKill;
	}
	else
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("UKillAllMode::Init - Template is not of type UKillAllMode"));
		return false;
	}
	 
	return true;
}

bool UKillAllMode::IsWaveComplete()
{
	if (!IsValid(WaveManager))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("KillAll::IsWaveComplete called but WaveManager is null"));
		return false;
	}

	if (!bGlobalKill)
	{
		return WaveManager->GetCurrentWaveAliveEnemiesCount() == 0;
	}
	
	return WaveManager->GetGlobalAliveEnemiesCount() == 0;
}
