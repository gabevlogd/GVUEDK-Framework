// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpawnSystem/SpawnMethods/StaticSpawnPoint.h"
#include "UObject/Object.h"
#include "StaticSpawnArea.generated.h"

/**
 *  A spawn method that returns a random location within a static box area on the navmesh (if possible)
 */
UCLASS()
class ENCOUNTERSYSTEM_API UStaticSpawnArea : public UStaticSpawnPoint
{
	GENERATED_BODY()

public:

	virtual bool Init(USpawnMethod* Template, USpawnManager* InSpawnManager) override;

	virtual bool FindSpawnLocation(UWorld* World, AActor* ContextActor, FVector& OutLocation) override;
	
	virtual void DrawGizmo(FPrimitiveDrawInterface* PDI) override;
	
	FBox GetAreaBox() const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	float Height = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	float Width = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	float Depth = 100.f;
	
};
