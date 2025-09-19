// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UCompassUtility.generated.h"

class UCompassTargetComponent;
class UCompassComponent;
/**
 * 
 */
UCLASS()
class COMPASSSYSTEM_API UCompassUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	
	static UCompassComponent* CompassComponent;

public:

	static void Init(UCompassComponent* InCompassComponent);

	UFUNCTION(BlueprintCallable, Category="Compass")
	static bool AddTarget(UCompassTargetComponent* CompassTarget);

	UFUNCTION(BlueprintCallable, Category="Compass")
	static bool RemoveTarget(const UCompassTargetComponent* CompassTarget);

	UFUNCTION(BlueprintCallable, Category="Compass")
	static void UpdateCardianlOrientation(float Orientation);
};
