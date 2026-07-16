// Copyright Villains, Inc. All Rights Reserved.


#include "Behaviours/SearchBehaviours/SphereTraceOnHook.h"

#include "GameFramework/Character.h"
#include "Components/GrapplingHookComponent.h"



bool USphereTraceOnHook::TickMode(float DeltaTime)
{
	Super::TickMode(DeltaTime);
	
	const FVector HookLocation = GrapplingHookComponent->GetHookLocation();
	
	// if we have not acquired a target and no obstacle was hit, we can look for one
	if (!IsTargetAcquired())
	{
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(OwnerCharacter);

		if (!PerformTargetDetection(HitResult, CollisionParams, HookLocation))
		{
			PerformObstacleDetection(HitResult, CollisionParams, HookLocation);
		}
	}
	
#if !UE_BUILD_SHIPPING
	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), HookLocation, GetGrabbingTolerance(), 12, IsTargetAcquired() ? FColor::Green : FColor::Red, false, 0.f);
		DrawDebugSphere(GetWorld(), HookLocation, ObstacleTolerance, 12, GrapplingHookComponent->HasHookHitObstacle() ? FColor::Green : FColor::Red, false, 0.f);
	}
#endif
	
	return true;
	
}

bool USphereTraceOnHook::PerformTargetDetection(FHitResult& HitResult, const FCollisionQueryParams& CollisionParams, const FVector& HookLocation)
{
	// Perform sphere trace arrownd the hook location to find a target
	if (GetWorld()->SweepSingleByChannel(HitResult, HookLocation, HookLocation, FQuat::Identity, GrabbableTraceChannel, FCollisionShape::MakeSphere(GetGrabbingTolerance()), CollisionParams))
	{
		if (HitResult.GetActor()->Implements<UGrabPoint>())
		{
			IGrabPoint* GrabPoint = Cast<IGrabPoint>(HitResult.GetActor());
			if (GrabPoint->Execute_CanBeGrabbed(HitResult.GetActor(), OwnerCharacter))
			{
				SetTargetGrabPoint(GrabPoint);
				GrabPoint->Execute_OnStartGrab(HitResult.GetActor());
				GrapplingHookComponent->OnHookAttached.Broadcast();
				return true;
			}
		}
	}
	return false;
}

bool USphereTraceOnHook::PerformObstacleDetection(FHitResult& HitResult, const FCollisionQueryParams& CollisionParams, const FVector& HookLocation)
{
	// Perform sphere trace arrownd the hook location to find an obstacle
	if (GetWorld()->SweepSingleByChannel(HitResult, HookLocation, HookLocation, FQuat::Identity, HookCollisionTraceChannel, FCollisionShape::MakeSphere(ObstacleTolerance), CollisionParams))
	{
		GrapplingHookComponent->SetHookHitObstacle(true);
		return true;
	}
	return false;
}

float USphereTraceOnHook::GetGrabbingTolerance()
{
	if (!IsValid(GrabbingToleranceCurve))
	{
		return LinearGrabbingTolerance;
	}
	return GrabbingToleranceCurve->GetFloatValue(FVector::Distance(GrapplingHookComponent->GetHookLocation(), OwnerCharacter->GetActorLocation()) * 0.01); // convert from cm to m
}
