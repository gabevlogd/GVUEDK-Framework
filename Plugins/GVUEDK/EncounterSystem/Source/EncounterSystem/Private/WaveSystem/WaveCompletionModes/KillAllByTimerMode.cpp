// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveCompletionModes/KillAllByTimerMode.h"
#include "WaveSystem/WaveManager.h"

bool UKillAllByTimerMode::Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager)
{
	if (Super::Init(Template, InWaveManager))
	{
		bCallCompletionCheckOnEnemyDeath = true;
		return true;
	}
	return false;
}

bool UKillAllByTimerMode::IsWaveComplete()
{
	// First, check if the timer has completed (control made by the parent UTimerMode)
	if (Super::IsWaveComplete())
	{
		if (IsValid(WaveManager))
		{
			// If the timer has completed, check if all enemies are dead
			if (WaveManager->GetGlobalAliveEnemiesCount() <= 0) 
			{
				return true;
			}

			// If there are still enemies alive after the timer completed, we consider the wave failed and stop it
			WaveManager->StopCurrentWave();
			return false;
		}
		
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("KillAllByTimerMode::IsWaveComplete called but WaveManager is null"));
		return false;
	}
	return false;
}
