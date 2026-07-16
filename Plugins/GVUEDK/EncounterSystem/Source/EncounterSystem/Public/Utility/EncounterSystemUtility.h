// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpawnSystem/SpawnManager.h"
#include "WaveSystem/WaveManager.h"
#include "EncounterSystemUtility.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogEncounterSystemUtility, All, All);

UCLASS()
class ENCOUNTERSYSTEM_API UEncounterSystemUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static void InitializeWaveSystem(UWaveManager* InWaveManager);

	static void DeinitializeWaveSystem();

	static void InitializeSpawnSystem(USpawnManager* InSpawnManager);

	static void DeinitializeSpawnSystem();

	UFUNCTION(BlueprintCallable)
	static void StartWave(UWaveData* WaveData, const int32 WaveIndex = 0);

	UFUNCTION(BlueprintCallable)
	static void StopCurrentWave();

	UFUNCTION(BlueprintCallable)
	static void StartNextWave();

	UFUNCTION(BlueprintCallable)
	static void SpawnWave(const UEnemyGroupData* EnemyGroupData);
	
	UFUNCTION(BlueprintCallable)
	static void SpawnGroup(const FEnemyGroup& EnemyGroup);

	UFUNCTION(BlueprintCallable)
	static AActor* SpawnEnemyAtLocation(const TSubclassOf<AActor> EnemyClass, const FVector& Location, const FRotator& Rotation);

	UFUNCTION(BlueprintCallable)
	static void StopPendingAsyncSpawns();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 GetGlobalAliveEnemiesCount();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetWaveRemainingTime();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetAllWavesRemainingTime();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsAnyWaveActive();
	

private:

	static UWaveManager* WaveManager;

	static USpawnManager* SpawnManager;
};
