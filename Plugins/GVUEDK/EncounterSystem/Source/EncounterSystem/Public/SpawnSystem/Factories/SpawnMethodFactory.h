// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpawnSystem/SpawnManager.h"
#include "UObject/Object.h"
#include "SpawnMethodFactory.generated.h"

class USpawnMethod;

DEFINE_LOG_CATEGORY_STATIC(LogSpawnMethodFactory, All, All);

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API USpawnMethodFactory : public UObject
{
	GENERATED_BODY()
	
public:

	static USpawnMethod* CreateSpawnMethod(USpawnMethod* Template, USpawnManager* InSpawnManager);
};
