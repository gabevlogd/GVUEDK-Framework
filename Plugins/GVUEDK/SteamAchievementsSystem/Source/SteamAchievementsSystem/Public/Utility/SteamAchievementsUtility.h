// Copyright The Prototypers, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamAchievementsUtility.generated.h"

/**
 * 
 */
UCLASS()
class STEAMACHIEVEMENTSSYSTEM_API USteamAchievementsUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	friend class USteamAchievementsSubsystem;
	
	static USteamAchievementsSubsystem* SteamAchievementsSubsystem;

public:

	UFUNCTION(BlueprintCallable)
	static bool UnlockAchievementRequest(const FName AchName,const float Progress);

	UFUNCTION(BlueprintCallable)
	static void SetMetaAchievementName(const FName& InMetaName);

private:
	
	static void Initialize(USteamAchievementsSubsystem* Subsystem);
	
	static bool Initialized() { return SteamAchievementsSubsystem != nullptr; }
};
