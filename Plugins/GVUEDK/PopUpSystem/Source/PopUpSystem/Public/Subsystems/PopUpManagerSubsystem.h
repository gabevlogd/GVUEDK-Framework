// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PopUpSystemSettings.h"
#include "Data/PopUpRegister.h"
#include "Subsystems/WorldSubsystem.h"
#include "PopUpManagerSubsystem.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogPopUpManagerSubsystem, All, All);

/**
 * 
 */
UCLASS()
class POPUPSYSTEM_API UPopUpManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:

	UPROPERTY()
	const UPopUpSystemSettings* PopUpSystemSettings;
	
	UPROPERTY()
	UPopUpRegister* PopUpRegister;

	UPROPERTY()
	TMap<FGameplayTag, UUserWidget*> ActivePopUps;

private:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:

	UUserWidget* AddPopUp(const FGameplayTag& PopUpTag);

	void RemovePopUp(const FGameplayTag& PopUpTag);

	void RemoveAllPopUps();

	void HidePopUp(const FGameplayTag& PopUpTag);

	void ShowPopUp(const FGameplayTag& PopUpTag);

	void HideAllPopUps();

	void ShowAllPopUps();

	bool TryGetActivePopUp(const FGameplayTag& PopUpTag, UUserWidget*& OutPopUpWidget) const;

	bool IsPopUpActive(const FGameplayTag& PopUpTag) const;
	

	
};
