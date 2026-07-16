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

class GRAPPLINGHOOKSYSTEM_API IGrabPoint
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	AActor* GetActor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	FVector GetLandingPoint();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	FVector GetLocation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	float GetMass();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	void SetLocation(const FVector& NewWorldLocation);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	bool CanBeGrabbed(const ACharacter* Character);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	void OnStartGrab();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	void OnMoving();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	void OnStopGrab();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	bool IsGrabbed();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	void Highlight();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	void Unhighlight();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	void OnBecameRelevant();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	void OnBecameGrabbable();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabPoint")
	bool IsStillRelevant();
};
