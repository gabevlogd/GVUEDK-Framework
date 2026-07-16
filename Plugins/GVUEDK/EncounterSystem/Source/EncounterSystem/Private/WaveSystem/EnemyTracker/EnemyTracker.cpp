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

	bAcceptingSpawns = true;

	// Listen for tracked enemies being destroyed to unregister them
	UTrackerComponent::OnTrackedEnemyDestroyed->AddUniqueDynamic(this, &UEnemyTracker::UnregisterEnemy);
	
	WaveManager = InWaveManager;
	
	// Listen for wave completion/cancellation to stop tracking enemies
	WaveManager->OnWaveCompleted.AddUniqueDynamic(this, &UEnemyTracker::StopEnemyTracking);

	AssociatedWaveID = WaveManager->GetCurrentWaveID();
}

void UEnemyTracker::Deinitialize()
{
	StopEnemyTracking(nullptr, -1);
	UTrackerComponent::OnTrackedEnemyDestroyed->RemoveDynamic(this, &UEnemyTracker::UnregisterEnemy);
	WaveManager = nullptr;
	TrackedEnemies.Empty();
}

void UEnemyTracker::RegisterEnemy(AActor* SpawnedEnemy)
{
	if (!IsValid(SpawnedEnemy) || !bAcceptingSpawns)
	{
		return;
	}
	
	if (!SpawnedEnemy->FindComponentByClass(UTrackerComponent::StaticClass()))
	{
		return;
	}
	
	UTrackerComponent::OnTrackedEnemyDestroyed->AddUniqueDynamic(this, &UEnemyTracker::UnregisterEnemy);
	
	TrackedEnemies.AddUnique(SpawnedEnemy);
}

void UEnemyTracker::UnregisterEnemy(AActor* DeadEnemy)
{
	if (!IsValid(DeadEnemy))
	{
		return;
	}

	const int32 RemovedCount = TrackedEnemies.RemoveSingleSwap(DeadEnemy);
	
	if (RemovedCount == 0)
	{
		return;
	}

	OnEnemyUnregistered.Broadcast(DeadEnemy);
	
	const bool bHasPendingAsyncSpawns = IsValid(WaveManager) && WaveManager->HasPendingAsyncSpawns();
	
	if (TrackedEnemies.IsEmpty() && !bHasPendingAsyncSpawns)
	{
		OnNoTrackedEnemiesLeft.Broadcast(AssociatedWaveID);
	}
	
	if (TrackedEnemies.IsEmpty() && !bAcceptingSpawns)
	{
		UTrackerComponent::OnTrackedEnemyDestroyed->RemoveDynamic(this, &UEnemyTracker::UnregisterEnemy);
	}
}

void UEnemyTracker::StopEnemyTracking(const UWaveData* WaveData, const int32 WaveIndex)
{
	bAcceptingSpawns = false;
	if (IsValid(WaveManager))
	{
		WaveManager->OnWaveCompleted.RemoveDynamic(this, &UEnemyTracker::StopEnemyTracking);
	}

	if (IsValid(GetWorld()))
	{
		if (USpawnManager* SpawnManager = GetWorld()->GetSubsystem<USpawnManager>())
		{
			SpawnManager->OnEnemySpawned.RemoveDynamic(this, &UEnemyTracker::RegisterEnemy);
		}
	}

	if (TrackedEnemies.IsEmpty())
	{
		UTrackerComponent::OnTrackedEnemyDestroyed->RemoveDynamic(this, &UEnemyTracker::UnregisterEnemy);
	}
}
