// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SequencesManagerData.h"
#include "UObject/Object.h"
#include "SequencesManagerSettings.generated.h"

/**
 * 
 */
UCLASS(Config = SequencesManagerSettings, DefaultConfig, meta = (DisplayName = "Sequences Manager Settings"))
class SEQUENCESMANAGERSYSTEM_API USequencesManagerSettings : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere, Category = Settings)
	TSoftObjectPtr<USequencesManagerData> SettingsData;

};
