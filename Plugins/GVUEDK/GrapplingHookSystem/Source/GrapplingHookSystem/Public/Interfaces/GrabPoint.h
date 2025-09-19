// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GrabPoint.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UGrabPoint : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GRAPPLINGHOOKSYSTEM_API IGrabPoint
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetActor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetLandingPoint();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetLocation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetMass();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetLocation(const FVector& NewWorldLocation);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanBeGrabbed(const ACharacter* Character);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnStartGrab();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnMoveing();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnStopGrab();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Highlight();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Unhighlight();
};
