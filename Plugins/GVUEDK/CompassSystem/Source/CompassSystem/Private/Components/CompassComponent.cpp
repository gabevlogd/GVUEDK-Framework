// Copyright Villains, Inc. All Rights Reserved.


#include "Components/CompassComponent.h"
#include "Utility/UCompassUtility.h"


UCompassComponent::UCompassComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UCompassComponent::AddTarget(UCompassTargetComponent* CompassTarget)
{
	if (!CompassTarget) return false;
	
	bool bAlreadyExists = false;
	CompassTargets.Add(CompassTarget, &bAlreadyExists);
	if (!bAlreadyExists)
	{
		CompassWidget->AddTarget(CompassTarget->GetTargetName(), CompassTarget->GetTargetIcon());
		return true;
	}
	return false;
}

bool UCompassComponent::RemoveTarget(const UCompassTargetComponent* CompassTarget)
{
	if (!CompassTarget) return false;
	
	const int Temp = CompassTargets.Remove(CompassTarget);
	if (Temp > 0)
	{
		CompassWidget->RemoveTarget(CompassTarget->GetTargetName());
		return true;
	}
	return false;
}

void UCompassComponent::OrientCompassWidget(float Orientation)
{
	if (!bInitialized)
	{
		return;
	}
	
	CompassWidget->UpdateCardinalOrientation(Orientation);
}

void UCompassComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	if (!bInitialized)
	{
		return;
	}
	
	UpdateCompassTargets();
	
	UpdateCardinalPoints();

	if (bOrientCompassWidget && PlayerController)
	{
		OrientCompassWidget(PlayerController->GetControlRotation().Yaw / 360.f);
	}
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCompassComponent::UpdateCompassTargets()
{
	for (const auto Element : CompassTargets)
	{
		if (!IsValid(Element))
		{
			RemoveTarget(Element);
			continue;
		}
		
		const FVector TargetLocation = Element->GetLocation();

		const float IconPosition = GetIconPosition(TargetLocation);
		
		FName Name = Element->GetTargetName();
		
		const bool bHideIconIfOutOfSight = Element->ShouldHideIconIfOutOfSight();
		
		CompassWidget->UpdateTargetIconPosition(Name, IconPosition, bHideIconIfOutOfSight);
	}
}

void UCompassComponent::UpdateCardinalPoints()
{
	for (const auto Element : CardinalPoints)
	{
		const FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraCacheView().Location;
		const FVector CardinalPointLocation = CameraLocation + CardinalDirections[Element.Name].RotateAngleAxis(CardinalPointOffsetDegrees, FVector::UpVector);

		const float IconPosition = GetIconPosition(CardinalPointLocation);

		FName Name = FName(*UEnum::GetValueAsName(Element.Name).ToString());

		CompassWidget->UpdateTargetIconPosition(Name, IconPosition, true);
	}
}

float UCompassComponent::GetIconPosition(const FVector& TargetLocation)
{
	if (!bInitialized)
	{
		return 0.f;
	}
	
	const FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraCacheView().Location;
	const FVector CameraForward = PlayerController->PlayerCameraManager->GetCameraCacheView().Rotation.Vector();
	
	FVector ToTarget = (TargetLocation - CameraLocation).GetSafeNormal2D();
	ToTarget.Z = 0.f;
	ToTarget.Normalize();

	FVector Forward = CameraForward;
	Forward.Z = 0.f;
	Forward.Normalize();

	FVector Right = FVector::CrossProduct(FVector::UpVector, Forward);

	float AngleRad = FMath::Atan2(FVector::DotProduct(ToTarget, Right), FVector::DotProduct(ToTarget, Forward));
	const float AngleDeg = FMath::RadiansToDegrees(AngleRad);
		
	const float RawAlpha = AngleRad / (PI / 2.f); // [-π/2, π/2] → [-1, 1]
		
	float Alpha = FMath::Clamp(RawAlpha, -1.f, 1.f);

	const bool bIsBehind = FMath::Abs(AngleDeg) > 90.f;
	if (bIsBehind)
	{
		Alpha = FMath::Sign(Alpha);
	}

	return Alpha;
}

void UCompassComponent::Init(UCompassWidget* InCompassWidget)
{
	CompassWidget = InCompassWidget;
	
	UCompassUtility::Init(this);

	InitCardinalPoints();

	if (GetWorld())
	{
		PlayerController = GetWorld()->GetFirstPlayerController();
	}
	
	bInitialized = PlayerController != nullptr && CompassWidget != nullptr;
}

void UCompassComponent::InitCardinalPoints()
{
	if (NorthIcon)
	{
		CardinalPoints.Add(FCardinalPoint(ECardinalPoint::North, NorthIcon));
	}
	if (SouthIcon)
	{
		CardinalPoints.Add(FCardinalPoint(ECardinalPoint::South, SouthIcon));
	}
	if (EastIcon)
	{
		CardinalPoints.Add(FCardinalPoint(ECardinalPoint::East, EastIcon));
	}
	if (WestIcon)
	{
		CardinalPoints.Add(FCardinalPoint(ECardinalPoint::West, WestIcon));
	}
	if (NorthEastIcon)
	{
		CardinalPoints.Add(FCardinalPoint(ECardinalPoint::NorthEast, NorthEastIcon));
	}
	if (NorthWestIcon)
	{
		CardinalPoints.Add(FCardinalPoint(ECardinalPoint::NorthWest, NorthWestIcon));
	}
	if (SouthEastIcon)
	{
		CardinalPoints.Add(FCardinalPoint(ECardinalPoint::SouthEast, SouthEastIcon));
	}
	if (SouthWestIcon)
	{
		CardinalPoints.Add(FCardinalPoint(ECardinalPoint::SouthWest, SouthWestIcon));
	}
	
	for (const auto Element : CardinalPoints)
	{
		CompassWidget->AddTarget(FName(*UEnum::GetValueAsName(Element.Name).ToString()), Element.Icon);
	}
}

