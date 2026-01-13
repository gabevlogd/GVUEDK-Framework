// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/SpawnMethod.h"
#include "UObject/Object.h"
#include "StaticSpawnPoint.generated.h"

/**
 *  A simple spawn method that returns a static location on the navmesh.
 */
UCLASS()
class ENCOUNTERSYSTEM_API UStaticSpawnPoint : public USpawnMethod
{
	GENERATED_BODY()
	
public:

	virtual bool Init(USpawnMethod* Template, USpawnManager* InSpawnManager) override;

	virtual bool FindSpawnLocation(UWorld* World, AActor* ContextActor, FVector& OutLocation) override;

	virtual bool FindSpawnRotation(UWorld* World, AActor* ContextActor, FRotator& OutRotation) override;

	virtual void DrawGizmo(FPrimitiveDrawInterface* PDI);

	virtual bool IsLocationOverlapping(UWorld* World, FVector Location);

	FVector GetSpawnPointLocation() const { return SpawnPointLocation; }

	void SetSpawnPointLocation(const FVector& NewLocation) { SpawnPointLocation = NewLocation; }

protected:

	/**
	 * The location of the spawn point.
	 * NOTE: This is not necessarily the final spawn location, as it will be projected to the navmesh within the defined extent.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	FVector SpawnPointLocation;

	/**
	 * The rotation to use for spawned actors
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	FRotator SpawnRotation;

	/**
	 * The extent around the spawn location to search for a valid navmesh location
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	FVector NavMeshSearchExtent = FVector(100.f);

	/**
	 * Minimum distance between spawned objects to avoid overlapping spawns
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	float MinSpawnDistance = 200.f;

	
	TArray<FVector> PreviousLocations;
};
