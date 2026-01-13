// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/EnemyGroupData.h"
#include "Data/WaveData.h"
#include "Settings/EncounterSystemSettings.h"
#include "Subsystems/WorldSubsystem.h"
#include "SpawnManager.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpawnManagerSubsystem, All, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemySpawned, AActor*, SpawnedEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopPendingSpawnRequestsDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAllGroupsSpawned);

UCLASS()
class ENCOUNTERSYSTEM_API USpawnManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FEnemySpawned OnEnemySpawned;
	
	FStopPendingSpawnRequestsDelegate OnStopPendingSpawnRequests;

	FAllGroupsSpawned OnAllGroupsSpawned;

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	
	AActor* SpawnEnemyAtLocation(const TSubclassOf<AActor> EnemyClass, const FVector& Location, const FRotator& Rotation) const;
	
	void SpawnGroup(const FEnemyGroup& EnemyGroup);

	void SpawnWave(const UEnemyGroupData* EnemyGroupData);
	
	void StopPendingSpawnRequests() const;
	
	void AsyncSpawnCompleted();

private:
	
	UFUNCTION()
	void SpawnWave(const UWaveData* WaveData, const int32 WaveIndex);
	
	UFUNCTION()
	void StopPendingSpawnRequests(const UWaveData* WaveData, const int32 WaveIndex);

	bool IsRangeZero(const FFloatRange& Range) const;

	int32 GetCountFromRange(const FInt32Range& Range) const;

private:

	UPROPERTY()
	const UEncounterSystemSettings* EncounterSystemSettings = nullptr;

	UPROPERTY()
	USpawnOperation* SpawnOperation = nullptr;

	int32 AsyncSpawnHandlerCount = 0;
};
