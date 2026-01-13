// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SpawnSystem/Factories/SpawnMethodFactory.h"
#include "SpawnMethod.generated.h"
/**
 *  Base class for defining spawn methods
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class ENCOUNTERSYSTEM_API USpawnMethod : public UObject
{
	GENERATED_BODY()
	
public:

	virtual bool Init(USpawnMethod* Template, USpawnManager* InSpawnManager);
	
	virtual bool FindSpawnLocation(UWorld* World, AActor* ContextActor, FVector& OutLocation) PURE_VIRTUAL(USpawnMethod::FindSpawnLocation, return false; );
	
	virtual bool FindSpawnRotation(UWorld* World, AActor* ContextActor, FRotator& OutRotation) PURE_VIRTUAL(USpawnMethod::FindSpawnRotation, return false; );

protected:

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Meta = (AdvancedDisplay, AllowPrivateAccess = "true"))
	bool bShowDebug = false;
#endif
};






