// Copyright Villains, Inc. All Rights Reserved.


#include "Utility/UCompassUtility.h"

#include "Components/CompassComponent.h"

UCompassComponent* UCompassUtility::CompassComponent = nullptr;

void UCompassUtility::Init(UCompassComponent* InCompassComponent)
{
	CompassComponent = InCompassComponent;
}

bool UCompassUtility::AddTarget(UCompassTargetComponent* CompassTarget)
{
	if (!CompassComponent)
	{
		return false;
	}

	return CompassComponent->AddTarget(CompassTarget);
}

bool UCompassUtility::RemoveTarget(const UCompassTargetComponent* CompassTarget)
{
	if (!CompassComponent)
	{
		return false;
	}

	return CompassComponent->RemoveTarget(CompassTarget);
}

void UCompassUtility::UpdateCardianlOrientation(float Orientation)
{
	if (!CompassComponent)
	{
		return;
	}

	CompassComponent->OrientCompassWidget(Orientation);
}
