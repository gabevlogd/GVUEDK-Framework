// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PopUpData.h"
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
	TArray<FPopUpData> PopUps;


	TMap<FGameplayTag, FPopUpData> GetPopUpDataMap() const
	{
		TMap<FGameplayTag, FPopUpData> PopUpDataMap;
		for (const FPopUpData& PopUpData : PopUps)
		{
			if (PopUpDataMap.Contains(PopUpData.Tag))
			{
				UE_LOG(LogTemp, Warning, TEXT("PopUpRegister: Duplicate PopUpTag %s found in PopUpRegister, skipping."), *PopUpData.Tag.ToString());
				continue;
			}

			if (!PopUpData.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("PopUpRegister: Invalid PopUpData for PopUpTag %s found in PopUpRegister, skipping."), *PopUpData.Tag.ToString());
				continue;
			}
			
			PopUpDataMap.Add(PopUpData.Tag, PopUpData);
		}
		return PopUpDataMap;
	}
	
};


