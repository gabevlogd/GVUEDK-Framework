// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/WaveData.h"
#include "UObject/Object.h"
#include "EnemyTracker.generated.h"

class UWaveManager;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyUnregistered, AActor*, DeadEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNoTrackedEnemiesLeft, const FGuid&, WaveID);

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UEnemyTracker : public UObject
{
	GENERATED_BODY()

public:
	
	FOnEnemyUnregistered OnEnemyUnregistered;

	FOnNoTrackedEnemiesLeft OnNoTrackedEnemiesLeft;
	
	void Init(UWaveManager* InWaveManager);

	void Deinitialize();

	UFUNCTION()
	void RegisterEnemy(AActor* SpawnedEnemy);

	UFUNCTION()
	void UnregisterEnemy(AActor* DeadEnemy);

	int32 GetTrackedEnemiesCount() const { return TrackedEnemies.Num(); }

private:

	UFUNCTION()
	void StopEnemyTracking(const UWaveData* WaveData, const int32 WaveIndex);

	UPROPERTY()
	TArray<AActor*> TrackedEnemies;

	UPROPERTY()
	UWaveManager* WaveManager;

	FGuid AssociatedWaveID;

	bool bAcceptingSpawns = false;
};
