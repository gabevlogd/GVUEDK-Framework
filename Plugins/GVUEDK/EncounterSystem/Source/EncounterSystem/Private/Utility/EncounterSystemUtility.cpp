// Copyright Villains, Inc. All Rights Reserved.


#include "Utility/EncounterSystemUtility.h"

UWaveManager* UEncounterSystemUtility::WaveManager = nullptr;

USpawnManager* UEncounterSystemUtility::SpawnManager = nullptr;

void UEncounterSystemUtility::InitializeWaveSystem(UWaveManager* InWaveManager)
{
	if (!IsValid(InWaveManager))
	{
		UE_LOG(LogEncounterSystemUtility, Warning, TEXT("InitializeWaveSystem called with null WaveManager"));
		return;
	}
	
	WaveManager = InWaveManager;
}

void UEncounterSystemUtility::DeinitializeWaveSystem()
{
	WaveManager = nullptr;
}

void UEncounterSystemUtility::InitializeSpawnSystem(USpawnManager* InSpawnManager)
{
	if (!IsValid(InSpawnManager))
	{
		UE_LOG(LogEncounterSystemUtility, Warning, TEXT("InitializeSpawnSystem called with null SpawnManager"));
		return;
	}
	
	SpawnManager = InSpawnManager;
}

void UEncounterSystemUtility::DeinitializeSpawnSystem()
{
	SpawnManager = nullptr;
}

void UEncounterSystemUtility::StartWave(UWaveData* WaveData, const int32 WaveIndex)
{
	if (IsValid(WaveManager))
	{
		WaveManager->StartWave(WaveData, WaveIndex);
	}
	else
	{
		UE_LOG(LogEncounterSystemUtility, Warning, TEXT("StartWave called but WaveManager is not initialized"));
	}
}

void UEncounterSystemUtility::StopCurrentWave()
{
	if (IsValid(WaveManager))
	{
		WaveManager->StopCurrentWave();
	}
	else
	{
		UE_LOG(LogEncounterSystemUtility, Warning, TEXT("StopCurrentWave called but WaveManager is not initialized"));
	}
}

void UEncounterSystemUtility::StartNextWave()
{
	if (IsValid(WaveManager))
	{
		WaveManager->StartNextWave();
	}
	else
	{
		UE_LOG(LogEncounterSystemUtility, Warning, TEXT("StartNextWave called but WaveManager is not initialized"));
	}
}

void UEncounterSystemUtility::SpawnWave(const UEnemyGroupData* EnemyGroupData)
{
	if (IsValid(SpawnManager))
	{
		SpawnManager->SpawnWave(EnemyGroupData);
	}
	else
	{
		UE_LOG(LogEncounterSystemUtility, Warning, TEXT("SpawnWave called but SpawnManager is not initialized"));
	}
}

void UEncounterSystemUtility::SpawnGroup(const FEnemyGroup& EnemyGroup)
{
	if (IsValid(SpawnManager))
	{
		SpawnManager->SpawnGroup(EnemyGroup);
	}
	else
	{
		UE_LOG(LogEncounterSystemUtility, Warning, TEXT("SpawnGroup called but SpawnManager is not initialized"));
	}
}

AActor* UEncounterSystemUtility::SpawnEnemyAtLocation(const TSubclassOf<AActor> EnemyClass, const FVector& Location,
	const FRotator& Rotation)
{
	if (IsValid(SpawnManager))
	{
		return SpawnManager->SpawnEnemyAtLocation(EnemyClass, Location, Rotation);
	}
	
	UE_LOG(LogEncounterSystemUtility, Warning, TEXT("SpawnEnemyAtLocation called but SpawnManager is not initialized"));
	return nullptr;
}

void UEncounterSystemUtility::StopPendingAsyncSpawns()
{
	if (IsValid(SpawnManager))
	{
		SpawnManager->StopPendingSpawnRequests();
	}
	else
	{
		UE_LOG(LogEncounterSystemUtility, Warning, TEXT("StopPendingAsyncSpawns called but SpawnManager is not initialized"));
	}
}

int32 UEncounterSystemUtility::GetGlobalAliveEnemiesCount()
{
	if (IsValid(WaveManager))
	{
		return WaveManager->GetGlobalAliveEnemiesCount();
	}
	
	UE_LOG(LogEncounterSystemUtility, Warning, TEXT("GetAliveEnemiesCount called but WaveManager is not initialized"));
	return -1;
}

float UEncounterSystemUtility::GetWaveRemainingTime()
{
	if (IsValid(WaveManager))
	{
		return WaveManager->GetWaveRemainingTime();
	}
	
	UE_LOG(LogEncounterSystemUtility, Warning, TEXT("GetWaveRemainingTime called but WaveManager is not initialized"));
	return -1.f;
}

float UEncounterSystemUtility::GetAllWavesRemainingTime()
{
	if (IsValid(WaveManager))
	{
		return WaveManager->GetAllWavesRemainingTime();
	}
	
	UE_LOG(LogEncounterSystemUtility, Warning, TEXT("GetAllWavesRemainingTime called but WaveManager is not initialized"));
	return -1.f;
}

bool UEncounterSystemUtility::IsAnyWaveActive()
{
	if (IsValid(WaveManager))
	{
		return WaveManager->IsAnyWaveActive();
	}
	
	UE_LOG(LogEncounterSystemUtility, Warning, TEXT("IsAnyWaveActive called but WaveManager is not initialized"));
	return false;
}


