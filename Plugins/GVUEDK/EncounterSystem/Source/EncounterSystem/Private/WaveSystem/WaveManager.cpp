// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveManager.h"

#include "Utility/EncounterSystemUtility.h"
#include "WaveSystem/WaveCompletionModes/TimerMode.h"
#include "WaveSystem/WaveCompletionModes/Base/WaveCompletionMode.h"
#include "WaveSystem/WaveExecutionModes/Base/WaveExecutionMode.h"
#include "WaveSystem/Factories/WaveCompletionModeFactory.h"
#include "WaveSystem/WaveEventsHandler/WaveEventsHandler.h"
#include "WaveSystem/Factories/WaveExecutionModeFactory.h"

void UWaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UEncounterSystemUtility::InitializeWaveSystem(this);

	WaveExecutionModeFactory = NewObject<UWaveExecutionModeFactory>(this);
	WaveCompletionModeFactory = NewObject<UWaveCompletionModeFactory>(this);
}

void UWaveManager::Deinitialize()
{
	Super::Deinitialize();

	UEncounterSystemUtility::DeinitializeWaveSystem();
}

void UWaveManager::StartWave(UWaveData* WaveData, const int32 WaveIndex)
{
	if (!IsValid(WaveData))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("StartWave called with null WaveData"));
		return;
	}
	
	if (!WaveData->Waves.IsValidIndex(WaveIndex))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("StartWave called with invalid WaveIndex %d"), WaveIndex);
		return;
	}

	if (!IsValid(WaveData->Waves[WaveIndex].WaveExecutionMode) ||
		!IsValid(WaveData->Waves[WaveIndex].WaveCompletionMode))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("StartWave called but WaveExecutionMode or WaveCompletionMode is null for wave index %d"), WaveIndex);
		return;
	}

	CurrentWaveExecutionMode = WaveExecutionModeFactory->CreateWaveExecutionMode(this, WaveData, WaveIndex);

	if (!IsValid(CurrentWaveExecutionMode))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("StartWave failed to create WaveExecutionMode for wave index %d"), WaveIndex);
		return;
	}

	CurrentWaveCompletionMode = WaveCompletionModeFactory->CreateWaveCompletionMode(this, WaveData, WaveIndex);

	if (!IsValid(CurrentWaveCompletionMode))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("StartWave failed to create WaveCompletionMode for wave index %d"), WaveIndex);
		return;
	}

	// listen to the spawn manager to know when all enemies have been spawned
	if (USpawnManager* SpawnManager = GetWorld()->GetSubsystem<USpawnManager>())
	{
		SpawnManager->OnAllGroupsSpawned.AddUniqueDynamic(this, &UWaveManager::AllEnemiesSpawned);
	}
	else
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("StartWave: Failed to get SpawnManager subsystem."));
		return;
	}

	bHasPendingAsyncSpawns = true;

	CurrentWaveID = FGuid::NewGuid();
	EnemyTrackers.Add(CurrentWaveID, CreateNewEnemyTracker());

	if (IsValid(WaveData->Waves[WaveIndex].WaveEventsHandler))
	{
		CurrentWaveEventsHandler = WaveData->Waves[WaveIndex].WaveEventsHandler;
		CurrentWaveEventsHandler->Init(this, WaveData, WaveIndex);
	}
	
	CurrentWaveExecutionMode->ExecuteWave();
}

void UWaveManager::StartNextWave()
{
	if (!IsValid(CurrentWaveExecutionMode))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("StartNextWave called but there is no current wave execution mode"));
		return;
	}

	if (!IsValid(CurrentWaveCompletionMode))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("StartNextWave called but there is no current wave completion mode"));
		return;
	}

	if (!CurrentWaveCompletionMode->IsWaveComplete())
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("StartNextWave called but the current wave is not yet complete"));
		return;
	}

	if (!CurrentWaveExecutionMode->HasNextWave())
	{
		AllWavesCompleted();
		CurrentWaveExecutionMode = nullptr;
		CurrentWaveCompletionMode = nullptr;
		CurrentWaveEventsHandler = nullptr;
		return;
	}
	
	StartWave(CurrentWaveExecutionMode->GetNextWaveData(), CurrentWaveExecutionMode->GetNextWaveIndex());
}

void UWaveManager::StopCurrentWave() 
{
	if (!IsValid(CurrentWaveExecutionMode))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("StopCurrentWave called but there is no current wave execution mode"));
		return;
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(NextWaveTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(NextWaveTimerHandle);
		NextWaveTimerHandle.Invalidate();
	}

	StopListeningSpawnManager();
	bHasPendingAsyncSpawns = false;
	CurrentWaveID.Invalidate();
	StopCallCompletionCheckOnEnemyDeath();
	WaveCanceled();
	CurrentWaveExecutionMode = nullptr;
	CurrentWaveCompletionMode = nullptr;
	CurrentWaveEventsHandler = nullptr;
}

int32 UWaveManager::GetGlobalAliveEnemiesCount() const
{
	int32 AliveEnemies = 0;
	for (const TPair<FGuid, UEnemyTracker*>& Pair : EnemyTrackers)
	{
		if (IsValid(Pair.Value))
		{
			AliveEnemies += Pair.Value->GetTrackedEnemiesCount();
		}
	}
	return AliveEnemies;
}

int32 UWaveManager::GetCurrentWaveAliveEnemiesCount() const
{
	if (!CurrentWaveID.IsValid() || !EnemyTrackers.Contains(CurrentWaveID))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("GetCurrentWaveAliveEnemiesCount called but there is no enemy tracker for the current wave ID"));
		return -1;
	}

	if (!IsValid(EnemyTrackers[CurrentWaveID]))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("GetCurrentWaveAliveEnemiesCount called but the enemy tracker for the current wave ID is null"));
		return -1;
	}

	return EnemyTrackers[CurrentWaveID]->GetTrackedEnemiesCount();
}

float UWaveManager::GetWaveRemainingTime() const
{
	if (!IsValid(CurrentWaveCompletionMode))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("GetWaveRemainingTime called but there is no current wave completion mode"));
		return -1.f;
	}

	if (const UTimerMode* TimerMode = Cast<UTimerMode>(CurrentWaveCompletionMode))
	{
		return TimerMode->GetWaveRemainingTime();
	}

	UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("GetWaveRemainingTime called but the current wave completion mode is not a TimerMode"));
	return -1.f;
}

UWaveData* UWaveManager::GetCurrentWaveData() const
{
	if (!IsValid(CurrentWaveExecutionMode))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("GetCurrentWaveData called but there is no current wave execution mode"));
		return nullptr;
	}

	return CurrentWaveExecutionMode->GetCurrentWaveData();
}

int32 UWaveManager::GetCurrentWaveIndex() const
{
	if (!IsValid(CurrentWaveExecutionMode))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("GetCurrentWaveIndex called but there is no current wave execution mode"));
		return -1;
	}

	return CurrentWaveExecutionMode->GetCurrentWaveIndex();
}

void UWaveManager::CheckWaveCompletion()
{
	if (!IsValid(CurrentWaveExecutionMode))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("CheckWaveCompletion called but there is no current wave execution mode"));
		return;
	}
	
	if (!IsValid(CurrentWaveCompletionMode))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("CheckWaveCompletion called but there is no current wave completion mode"));
		return;
	}

	if (CurrentWaveCompletionMode->IsWaveComplete())
	{
		StopCallCompletionCheckOnEnemyDeath();
		StopListeningSpawnManager();
		WaveCompleted();
	}
}

void UWaveManager::CallCompletionCheckOnEnemyDeath(const bool ListenToEveryTracker)
{
	if (ListenToEveryTracker)
	{
		for (const TPair<FGuid, UEnemyTracker*>& Pair : EnemyTrackers)
		{
			if (IsValid(Pair.Value))
			{
				Pair.Value->OnEnemyUnregistered.AddUniqueDynamic(this, &UWaveManager::CheckWaveCompletion);
			}
		}
	}
	else
	{
		if (!CurrentWaveID.IsValid() || !EnemyTrackers.Contains(CurrentWaveID))
		{
			UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("CallCompletionCheckOnEnemyDeath called but there is no enemy tracker for the current wave ID"));
			return;
		}

		if (!IsValid(EnemyTrackers[CurrentWaveID]))
		{
			UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("CallCompletionCheckOnEnemyDeath called but the enemy tracker for the current wave ID is null"));
			return;
		}

		EnemyTrackers[CurrentWaveID]->OnEnemyUnregistered.AddUniqueDynamic(this, &UWaveManager::CheckWaveCompletion);
	}
}

void UWaveManager::StopCallCompletionCheckOnEnemyDeath() const
{
	for (const TPair<FGuid, UEnemyTracker*>& Pair : EnemyTrackers)
	{
		if (IsValid(Pair.Value))
		{
			Pair.Value->OnEnemyUnregistered.RemoveDynamic(this, &UWaveManager::CheckWaveCompletion);
		}
	}
}

void UWaveManager::StopListeningSpawnManager()
{
	if (USpawnManager* SpawnManager = GetWorld()->GetSubsystem<USpawnManager>())
	{
		SpawnManager->OnAllGroupsSpawned.RemoveDynamic(this, &UWaveManager::AllEnemiesSpawned);
	}
}

UEnemyTracker* UWaveManager::CreateNewEnemyTracker()
{
	UEnemyTracker* NewEnemyTracker = NewObject<UEnemyTracker>(this);
	NewEnemyTracker->Init(this);
	NewEnemyTracker->OnEnemyUnregistered.AddUniqueDynamic(this, &UWaveManager::EnemyDead);
	NewEnemyTracker->OnNoTrackedEnemiesLeft.AddUniqueDynamic(this, &UWaveManager::RemoveEnemyTracker);
	return NewEnemyTracker;
}

void UWaveManager::WaveStarted()
{
	OnWaveStarted.Broadcast(CurrentWaveExecutionMode->GetCurrentWaveData(), CurrentWaveExecutionMode->GetCurrentWaveIndex());
	if (IsValid(CurrentWaveEventsHandler))
	{
		CurrentWaveEventsHandler->OnWaveStarted(this, CurrentWaveExecutionMode->GetCurrentWaveData(), CurrentWaveExecutionMode->GetCurrentWaveIndex());
	}
}

void UWaveManager::WaveCompleted()
{
	OnWaveCompleted.Broadcast(CurrentWaveExecutionMode->GetCurrentWaveData(), CurrentWaveExecutionMode->GetCurrentWaveIndex());
	if (IsValid(CurrentWaveEventsHandler))
	{
		CurrentWaveEventsHandler->OnWaveCompleted(this, CurrentWaveExecutionMode->GetCurrentWaveData(), CurrentWaveExecutionMode->GetCurrentWaveIndex());
	}
	
	if (CurrentWaveExecutionMode->ShouldAutoStartNextWave())
	{
		AutoStartNextWave();
	}
	
}

void UWaveManager::WaveCanceled()
{
	OnWaveCanceled.Broadcast(CurrentWaveExecutionMode->GetCurrentWaveData(), CurrentWaveExecutionMode->GetCurrentWaveIndex());
	if (IsValid(CurrentWaveEventsHandler))
	{
		CurrentWaveEventsHandler->OnWaveCanceled(this, CurrentWaveExecutionMode->GetCurrentWaveData(), CurrentWaveExecutionMode->GetCurrentWaveIndex());
	}
}

void UWaveManager::AllWavesCompleted()
{
	OnAllWavesCompleted.Broadcast(CurrentWaveExecutionMode->GetCurrentWaveData(), CurrentWaveExecutionMode->GetCurrentWaveIndex());
	if (IsValid(CurrentWaveEventsHandler))
	{
		CurrentWaveEventsHandler->OnAllWavesCompleted(this, CurrentWaveExecutionMode->GetCurrentWaveData(), CurrentWaveExecutionMode->GetCurrentWaveIndex());
	}
}

void UWaveManager::RemoveEnemyTracker(const FGuid& WaveID)
{
	if (EnemyTrackers.Contains(WaveID))
	{
		EnemyTrackers[WaveID]->OnNoTrackedEnemiesLeft.RemoveDynamic(this, &UWaveManager::RemoveEnemyTracker);
		EnemyTrackers.Remove(WaveID);
	}
}

void UWaveManager::CheckWaveCompletion(AActor* LastDeadEnemy)
{
	CheckWaveCompletion();
}

void UWaveManager::AutoStartNextWave()
{
	if (const float NextWaveDelay = CurrentWaveExecutionMode->GetDelayBeforeNextWave(); NextWaveDelay > KINDA_SMALL_NUMBER)
	{
		NextWaveTimerHandle = FTimerHandle();
		GetWorld()->GetTimerManager().SetTimer(NextWaveTimerHandle, this, &UWaveManager::StartNextWave, NextWaveDelay, false);
		return;
	}

	StartNextWave();
}

void UWaveManager::AllEnemiesSpawned()
{
	bHasPendingAsyncSpawns = false;
	if (CurrentWaveCompletionMode->ShouldCallCompletionCheckOnEnemyDeath())
	{
		CallCompletionCheckOnEnemyDeath();
	}
	OnAllEnemiesSpawned.Broadcast();
	if (IsValid(CurrentWaveEventsHandler))
	{
		CurrentWaveEventsHandler->OnAllEnemiesSpawned(this, CurrentWaveExecutionMode->GetCurrentWaveData(), CurrentWaveExecutionMode->GetCurrentWaveIndex());
	}
}

void UWaveManager::EnemyDead(AActor* DeadEnemy)
{
	OnEnemyDead.Broadcast(DeadEnemy);
	if (IsValid(CurrentWaveEventsHandler))
	{
		CurrentWaveEventsHandler->OnEnemyDead(this, CurrentWaveExecutionMode->GetCurrentWaveData(), CurrentWaveExecutionMode->GetCurrentWaveIndex(), DeadEnemy);
	}
}
