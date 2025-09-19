// Copyright Villains, Inc. All Rights Reserved.


#include "Components/EnvironmentTracing/GroundCheckComponent.h"


UGroundCheckComponent::UGroundCheckComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	StartTraceLocation = FVector::ZeroVector;
}

float UGroundCheckComponent::CheckGroundDistance() const
{
	const FVector Start = GetOwner()->GetActorTransform().TransformPosition(StartTraceLocation);
	const FVector End = Start + FVector::DownVector * MaxGroundDistance;

	FHitResult HitResult;
	if (PerformSphereTrace(Start, End, SphereCastRadius, HitResult, GroundTraceChannel))
	{
		return (Start - HitResult.ImpactPoint).Size();
	}
	return -1.0f;
}

bool UGroundCheckComponent::IsGrounded(const float Tolerance) const
{
	const float GroundDistance = CheckGroundDistance();
	return GroundDistance <= FMath::Max(Tolerance, 0.f) && GroundDistance > -1.0f;
}

