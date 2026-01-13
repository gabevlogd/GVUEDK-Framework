// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpawnSystem/SpawnOperation.h"
#include "UObject/Object.h"
#include "EncounterSystemSettings.generated.h"

/**
 * 
 */
UCLASS(Config = EncounterSystemSettings, DefaultConfig, meta = (DisplayName = "Encounter System Settings"))
class ENCOUNTERSYSTEM_API UEncounterSystemSettings : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere, Category = Settings)
	TSubclassOf<USpawnOperation> DefaultSpawnOperation = USpawnOperation::StaticClass();
};
