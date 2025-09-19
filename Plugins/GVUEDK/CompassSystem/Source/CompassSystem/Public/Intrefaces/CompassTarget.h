// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CompassTarget.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCompassTarget : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COMPASSSYSTEM_API ICompassTarget 
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCompassTargetLocation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UObject* GetIcon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HideIconIfOutOfSight();
};
