// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/PopUpManagerSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PopUpSystemUtility.generated.h"
/**
 * 
 */
UCLASS()
class POPUPSYSTEM_API UPopUpSystemUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	
	static UPopUpManagerSubsystem* PopUpManager;

public:

	static void Init(UPopUpManagerSubsystem* InPopUpManager);

	UFUNCTION(BlueprintCallable)
	static UUserWidget* AddPopUp(const FGameplayTag PopUpTag);

	UFUNCTION(BlueprintCallable)
	static void RemovePopUp(const FGameplayTag PopUpTag);
	
	UFUNCTION(BlueprintCallable)
	static void RemoveAllPopUps();

	UFUNCTION(BlueprintCallable)
	static void HidePopUp(const FGameplayTag PopUpTag);

	UFUNCTION(BlueprintCallable)
	static void ShowPopUp(const FGameplayTag PopUpTag);

	UFUNCTION(BlueprintCallable)
	static void HideAllPopUps();

	UFUNCTION(BlueprintCallable)
	static void ShowAllPopUps();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool TryGetActivePopUp(const FGameplayTag PopUpTag, UUserWidget*& OutPopUpWidget);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsPopUpActive(const FGameplayTag PopUpTag);
	
};
