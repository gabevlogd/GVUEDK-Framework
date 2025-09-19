// Copyright Villains, Inc. All Rights Reserved.


#include "Components/EnvironmentTracing/Base/EnvironmentTracingComponent.h"

#include "Utility/FGvDebug.h"


UEnvironmentTracingComponent::UEnvironmentTracingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnvironmentTracingComponent::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	if (!World)
	{
		bInitialized = false;
		return;
	}

	if (bIgnoreSelf)
	{
		QueryParams.AddIgnoredActor(GetOwner());
	}
	bInitialized = true;
}

bool UEnvironmentTracingComponent::PerformLineTrace(const FVector& Start, const FVector& End,
                                                    FHitResult& OutHitResult, ECollisionChannel TraceChannel) const
{
	//To Do
	FGvDebug::Error("Implement PerformLineTrace", true);
	return false;
}

bool UEnvironmentTracingComponent::PerformSphereTrace(const FVector& Start, const FVector& End, float Radius,
                                                      FHitResult& OutHitResult, ECollisionChannel TraceChannel) const
{
	if (!bInitialized)
	{
		FGvDebug::Error(GetName() + " is not initialized");
		return false;
	}

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Radius);
	bool bHit = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, TraceChannel, CollisionShape,
	                                            QueryParams);
	
#if WITH_EDITOR
	if (bShowDebug)
	{
		DrawDebugSphere(World, Start, Radius, 12, FColor::Red, false, -1);
		if (!bHit)
		{
			DrawDebugSphere(World, End, Radius, 12, FColor::Red, false, -1);
			DrawDebugLine(World, Start, End, FColor::Red, false, -1);
		}
		else
		{
			DrawDebugSphere(World, OutHitResult.ImpactPoint, Radius, 12, FColor::Green, false, -1);
			DrawDebugLine(World, Start, OutHitResult.ImpactPoint, FColor::Red, false, -1);
			DrawDebugLine(World, OutHitResult.ImpactPoint, End, FColor::Green, false, -1);
		}
	}
#endif

	return bHit;
	
}
