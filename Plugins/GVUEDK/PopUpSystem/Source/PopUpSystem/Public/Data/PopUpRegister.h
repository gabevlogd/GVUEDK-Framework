// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "PopUpRegister.generated.h"

/**
 * 
 */
UCLASS()
class POPUPSYSTEM_API UPopUpRegister : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PopUp Register")
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> PopUpRegisterItems;
};


