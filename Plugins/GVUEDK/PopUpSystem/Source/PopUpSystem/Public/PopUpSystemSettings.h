// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/PopUpRegister.h"
#include "UObject/Object.h"
#include "PopUpSystemSettings.generated.h"

/**
 * 
 */
UCLASS(Config = PopUpSystemSettings, DefaultConfig, meta = (DisplayName = "Pop Up System Settings"))
class POPUPSYSTEM_API UPopUpSystemSettings : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere, Category = Settings)
	TSoftObjectPtr<UPopUpRegister> PopUpRegister;

public:

	explicit UPopUpSystemSettings(const FObjectInitializer& ObjectInitializer);
	
};
