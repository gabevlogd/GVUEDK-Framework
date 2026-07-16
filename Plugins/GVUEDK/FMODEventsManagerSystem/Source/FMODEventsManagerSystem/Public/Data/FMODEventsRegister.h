// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODEvent.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FMODEventsRegister.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FMODEVENTSMANAGERSYSTEM_API UFMODEventsRegister : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Soundtrack Register")
	TMap<FGameplayTag, UFMODEvent*> GameSoundtracks;
};
