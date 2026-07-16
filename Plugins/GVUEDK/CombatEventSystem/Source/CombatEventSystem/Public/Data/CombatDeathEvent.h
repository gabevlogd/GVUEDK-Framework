// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatDeathEvent.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCombatDeathEvent
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, Category = "CombatDeathEvent")
	TObjectPtr<AActor> Killer;

	UPROPERTY(BlueprintReadWrite, Category = "CombatDeathEvent")
	TObjectPtr<AActor> Victim;

	UPROPERTY(BlueprintReadWrite, Category = "CombatDeathEvent")
	UDamageType* DamageType = nullptr;
};
