// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatEventSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatEventUtility.generated.h"

/**
 * 
 */
UCLASS()
class COMBATEVENTSYSTEM_API UCombatEventUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static void Initialize(UCombatEventSubsystem* InCombatEventSubsystem);

	UFUNCTION(BlueprintCallable, Category = "Combat Event")
	static void NotifyCombatDeathEvent(const FCombatDeathEvent& CombatDeathEvent);

private:
	
	static UCombatEventSubsystem* CombatEventSubsystem;
};
