// Copyright Villains, Inc. All Rights Reserved.


#include "Behaviours/SearchBehaviours/GHSphereTraceOnOwner.h"

#include "Components/GrapplingHookComponent.h"
#include "GameFramework/Character.h"

UGHSphereTraceOnOwner::UGHSphereTraceOnOwner()
{
	bShowDebug = false;
	MaxDistance = 3000.f;
	MinDistance = 0.f;
	LastTargetGrabPoint = nullptr;
	GrabPointsChannel = ECC_GameTraceChannel1;
}

void UGHSphereTraceOnOwner::Initialize(UGrapplingHookComponent* InGrapplingHookComponent)
{
	Super::Initialize(InGrapplingHookComponent);
	InRangeGrabPoints = TSet<IGrabPoint*>();
}

bool UGHSphereTraceOnOwner::TickMode(float DeltaTime)
{
	Super::TickMode(DeltaTime);
	// check if there are any grab points in range (MaxCheckDistance)
	if (LookForGrabPoints(InRangeGrabPoints))
	{
		SetTargetGrabPoint(GetMostRelevantGrabPoint(InRangeGrabPoints));
		if (IsTargetAcquired())
		{
			if (LastTargetGrabPoint != nullptr && LastTargetGrabPoint != GetTargetGrabPoint())
			{
				LastTargetGrabPoint->Execute_Unhighlight(LastTargetGrabPoint->_getUObject());
			}
			if (LastTargetGrabPoint != GetTargetGrabPoint())
				GetTargetGrabPoint()->Execute_Highlight(GetTargetGrabPoint()->_getUObject());
			
			LastTargetGrabPoint = GetTargetGrabPoint();
		}
		else
		{
			if (LastTargetGrabPoint != nullptr)
			{
				LastTargetGrabPoint->Execute_Unhighlight(LastTargetGrabPoint->_getUObject());
				LastTargetGrabPoint = nullptr;
			}
		}
		return true;
	}
	// if there are no grab points in range, reset the target
	if (IsTargetAcquired())
	{
		GetTargetGrabPoint()->Execute_Unhighlight(GetTargetGrabPoint()->_getUObject());
		LastTargetGrabPoint = nullptr;
		SetTargetGrabPoint(nullptr);
		return true;
	}
	return false;
}

void UGHSphereTraceOnOwner::ExitMode()
{
	if (GetTargetGrabPoint() != nullptr)
	{
		GetTargetGrabPoint()->Execute_Unhighlight(GetTargetGrabPoint()->_getUObject());
	}
	if (LastTargetGrabPoint != nullptr)
	{
		LastTargetGrabPoint->Execute_Unhighlight(LastTargetGrabPoint->_getUObject());
		LastTargetGrabPoint = nullptr;
	}
	Super::ExitMode();
}

bool UGHSphereTraceOnOwner::LookForGrabPoints(TSet<IGrabPoint*>& OutGrabPoints) const
{
	TArray<FHitResult> HitResults;
	if (PerformSphereTrace(HitResults))
	{
		for (auto HitResult : HitResults)
		{
			IGrabPoint* GrabPoint = Cast<IGrabPoint>(HitResult.GetActor());
			
			if (GrabPoint == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("%s is not a grab point, FIX: set ignore collision response"), *HitResult.GetActor()->GetName());
				continue;
			}
			// check if the grab point is in range and can be grabbed (TO DO: Check if DistSquared can be moved in CanBeGrabbed)
			if (FVector::DistSquared(OwnerCharacter->GetActorLocation(), GrabPoint->Execute_GetLocation(HitResult.GetActor())) <= MaxDistance * MaxDistance
				&& GrabPoint->Execute_CanBeGrabbed(HitResult.GetActor(), OwnerCharacter))
			{
				OutGrabPoints.Add(GrabPoint);
			}
			else
			{
				OutGrabPoints.Remove(GrabPoint);
			}
			
		}
		return true;
	}
	return false;
}

IGrabPoint* UGHSphereTraceOnOwner::GetMostRelevantGrabPoint(TSet<IGrabPoint*>& ValidGrabPoints) const
{
	IGrabPoint* ReturnValue = nullptr;
	float MinSquaredDistance = BIG_NUMBER;
	for (IGrabPoint* GrabPoint : ValidGrabPoints)
	{
		const float SquaredDistance = FVector::DistSquared(GrabPoint->Execute_GetLocation(GrabPoint->_getUObject()), OwnerCharacter->GetActorLocation());
		if (MinSquaredDistance > SquaredDistance)
		{
			MinSquaredDistance = SquaredDistance;
			ReturnValue = GrabPoint;
		}
	}
	return ReturnValue;
}

bool UGHSphereTraceOnOwner::PerformSphereTrace(TArray<FHitResult>& HitResults) const
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(MaxDistance);

	
	const bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults, 
		OwnerCharacter->GetActorLocation(),
		OwnerCharacter->GetActorLocation(),
		FQuat::Identity,
		GrabPointsChannel,
		CollisionShape
	);

#if !UE_BUILD_SHIPPING
	if (bShowDebug)
	{
		DrawDebugSphere(
			GetWorld(),
			OwnerCharacter->GetActorLocation(),
			MaxDistance,
			15,
			bHit ? FColor::Green : FColor::Red,
			false,
			-1
		);
	}
#endif

	return bHit;
}
