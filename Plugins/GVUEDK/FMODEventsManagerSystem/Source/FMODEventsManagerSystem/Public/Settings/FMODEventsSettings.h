// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/FMODEventsRegister.h"
#include "UObject/Object.h"
#include "FMODEventsSettings.generated.h"

/**
 * 
 */
UCLASS(Config = FMODEventsSettings, DefaultConfig, meta = (DisplayName = "FMOD Events Manager System Settings"))
class FMODEVENTSMANAGERSYSTEM_API UFMODEventsSettings : public UObject
{
	GENERATED_BODY()

public:

	
	UPROPERTY(Config, EditAnywhere, Category = Settings)
	TSoftObjectPtr<UFMODEventsRegister> FMODEventsRegister;
};
