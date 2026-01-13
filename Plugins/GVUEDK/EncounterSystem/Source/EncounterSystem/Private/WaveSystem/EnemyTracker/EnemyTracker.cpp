// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/EnemyTracker/EnemyTracker.h"
#include "SpawnSystem/SpawnManager.h"
#include "WaveSystem/WaveManager.h"
#include "WaveSystem/EnemyTracker/TrackerComponent.h"

void UEnemyTracker::Init(UWaveManager* InWaveManager)
{
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("EnemyTracker Init called but World is null"));
		return;
	}

	if (!IsValid(InWaveManager))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("EnemyTracker Init called but WaveManager is null"));
		return;
	}
	
	// Listen for spawned enemies to register them
	if (USpawnManager* SpawnManager = GetWorld()->GetSubsystem<USpawnManager>())
	{
		SpawnManager->OnEnemySpawned.AddUniqueDynamic(this, &UEnemyTracker::RegisterEnemy);
	}
	else
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("EnemyTracker Init: Failed to get SpawnManager subsystem."));
	}

	// Listen for tracked enemies being destroyed to unregister them
	UTrackerComponent::OnTrackedEnemyDestroyed->AddUniqueDynamic(this, &UEnemyTracker::UnregisterEnemy);
	
	WaveManager = InWaveManager;
	
	// Listen for wave completion/cancellation to stop tracking enemies
	WaveManager->OnWaveCompleted.AddUniqueDynamic(this, &UEnemyTracker::StopEnemyTracking);
	WaveManager->OnWaveCanceled.AddUniqueDynamic(this, &UEnemyTracker::StopEnemyTracking);

	AssociatedWaveID = WaveManager->GetCurrentWaveID();
}

void UEnemyTracker::RegisterEnemy(AActor* SpawnedEnemy)
{
	if (SpawnedEnemy->FindComponentByClass(UTrackerComponent::StaticClass()))
	{
		TrackedEnemies.AddUnique(SpawnedEnemy);
	}
}

void UEnemyTracker::UnregisterEnemy(AActor* DeadEnemy)
{
	if (TrackedEnemies.Contains(DeadEnemy))
	{
		TrackedEnemies.Remove(DeadEnemy);
		OnEnemyUnregistered.Broadcast(DeadEnemy);
	}

	// If no tracked enemies are left, broadcast the event, unless there are pending async spawns to avoid premature completion
	if (TrackedEnemies.Num() == 0 && !WaveManager->HasPendingAsyncSpawns())
	{
		UTrackerComponent::OnTrackedEnemyDestroyed->RemoveDynamic(this, &UEnemyTracker::UnregisterEnemy);
		OnNoTrackedEnemiesLeft.Broadcast(AssociatedWaveID);
	}
}

void UEnemyTracker::StopEnemyTracking(const UWaveData* WaveData, const int32 WaveIndex)
{
	WaveManager->OnWaveCompleted.RemoveDynamic(this, &UEnemyTracker::StopEnemyTracking);
	WaveManager->OnWaveCanceled.RemoveDynamic(this, &UEnemyTracker::StopEnemyTracking);
	
	if (USpawnManager* SpawnManager = GetWorld()->GetSubsystem<USpawnManager>())
	{
		SpawnManager->OnEnemySpawned.RemoveDynamic(this, &UEnemyTracker::RegisterEnemy);
	}
}
