// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/CombatDeathEvent.h"
#include "Subsystems/WorldSubsystem.h"
#include "CombatEventSubsystem.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogCombatEventSystem, All, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathEvent, const FCombatDeathEvent&, CombatDeathEvent);

/**
 * 
 */
UCLASS()
class COMBATEVENTSYSTEM_API UCombatEventSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category="CombatEventSubsystem")
	FOnDeathEvent OnDeathEvent;

private:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
