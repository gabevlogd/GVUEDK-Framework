// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "CompassWidget.generated.h"

class ICompassTarget;
/**
 * 
 */
UCLASS()
class COMPASSSYSTEM_API UCompassWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compass Widget")
	FVector2D DefaultTargetIconsSize = FVector2D(64.f, 64.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compass Widget")
	float TargetIconsVerticalOffset = 0.f;

private:

	UPROPERTY()
	TMap<FName, UImage*> CompassTargets;

public:

	UFUNCTION(BlueprintImplementableEvent, Category="Compass")
	void UpdateCardinalOrientation(float Orientation);

	void AddTarget(const FName& Name, UObject* TargetIcon);

	void RemoveTarget(const FName& Name);

	UFUNCTION(BlueprintCallable, Category="Compass")
	void UpdateTargetIconPosition(const FName& Name, const float Position, const bool bHideIconIfOutOfSight = false);
};
