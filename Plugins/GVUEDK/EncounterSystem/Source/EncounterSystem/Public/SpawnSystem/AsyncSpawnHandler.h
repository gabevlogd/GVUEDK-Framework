// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpawnSystem/SpawnManager.h"
#include "UObject/Object.h"
#include "AsyncSpawnHandler.generated.h"

/**
 *  Struct representing a single enemy spawn request with class, location, and rotation.
 */
USTRUCT()
struct FSpawnRequest
{
	GENERATED_BODY()

	FSpawnRequest()
		: EnemyClass(nullptr)
		, Location(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator) {}

	FSpawnRequest(const TSubclassOf<AActor> InEnemyClass, const FVector& InLocation, const FRotator& InRotation)
		: EnemyClass(InEnemyClass)
		, Location(InLocation)
		, Rotation(InRotation) {}

	TSubclassOf<AActor> EnemyClass;

	FVector Location;

	FRotator Rotation;
}; 

/**
 *  Handles asynchronous spawning of enemies with specified delays and intervals.
 */
UCLASS()
class ENCOUNTERSYSTEM_API UAsyncSpawnHandler : public UObject
{
	GENERATED_BODY()

public:

	UAsyncSpawnHandler();

	void Init(USpawnManager* InSpawnManager, const FFloatRange InSpawnDelayRange = FFloatRange(0.f, 0.f), const FFloatRange InSpawnIntervalRange = FFloatRange(0.f, 0.f));
	
	/**
	 *  Recursively spawns enemies from the SpawnRequests list with the specified delay and interval, until all requests are processed.
	 *  Then deinitializes itself.
	 */
	void AsyncSpawn();

	void AddSpawnRequest(const FSpawnRequest& SpawnRequest);
	
private:

	UFUNCTION()
	void Deinitialize();

	void OnWorldCleanup(UWorld* World, bool bArg, bool bCond);

	float GetValidFloatInRange(FFloatRange InRange);
	
	UPROPERTY()
	USpawnManager* SpawnManager;

	TArray<FSpawnRequest> SpawnRequests;

	FTimerHandle SpawnTimer;

	float SpawnDelay = -1.f;

	FFloatRange SpawnIntervalRange = FFloatRange(-1.f, -1.f);

	FTimerDelegate SpawnDelegateHandle;

	FDelegateHandle WorldCleanupDelegateHandle;
};
