// Copyright Villains, Inc. All Rights Reserved.


#include "Behaviours/SearchBehaviours/SphereTraceOnOwner.h"

#include "Components/GrapplingHookComponent.h"
#include "GameFramework/Character.h"

USphereTraceOnOwner::USphereTraceOnOwner()
{
	bShowDebug = false;
	RelevancyDistance = 4000.f;
	MaxGrabDistance = 3000.f;
	MinGrabDistance = 0.f;
	LastTargetGrabPoint = nullptr;
	GrabPointsChannel = ECC_GameTraceChannel1;
}

void USphereTraceOnOwner::Initialize(UGrapplingHookComponent* InGrapplingHookComponent)
{
	Super::Initialize(InGrapplingHookComponent);
	RelevantGrabPoints = TSet<TWeakInterfacePtr<IGrabPoint>>();
}

bool USphereTraceOnOwner::TickMode(float DeltaTime)
{
	Super::TickMode(DeltaTime);

	if (LookForGrabPoints(RelevantGrabPoints))
	{
		TWeakInterfacePtr<IGrabPoint> Target = TryGetTargetGrabPoint();
		if (Target != nullptr)
		{
			SetTargetGrabPoint(Target.Get());
			Target->Execute_OnBecameGrabbable(Target->_getUObject());
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

void USphereTraceOnOwner::ExitMode()
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

bool USphereTraceOnOwner::LookForGrabPoints(TSet<TWeakInterfacePtr<IGrabPoint>>& OutGrabPoints) const
{
	TArray<FHitResult> HitResults;
	if (PerformSphereTrace(HitResults))
	{
		for (auto HitResult : HitResults)
		{
			TWeakInterfacePtr GrabPoint = Cast<IGrabPoint>(HitResult.GetActor());
			
			if (!GrabPoint.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("%s is not a grab point, FIX: set ignore collision response"), *HitResult.GetActor()->GetName());
				continue;
			}

			if (!GrabPoint->Execute_IsStillRelevant(GrabPoint->_getUObject()))
			{
				GrabPoint->Execute_OnBecameRelevant(GrabPoint->_getUObject());
			}

			OutGrabPoints.Add(GrabPoint);
		}
		return true;
	}
	return false;
}

TWeakInterfacePtr<IGrabPoint> USphereTraceOnOwner::GetMostRelevantGrabPoint(TSet<TWeakInterfacePtr<IGrabPoint>> OutGrabPoints) const
{
	TWeakInterfacePtr<IGrabPoint> ReturnValue = nullptr;
	float MinSquaredDistance = BIG_NUMBER;
	for (TWeakInterfacePtr GrabPoint : OutGrabPoints)
	{
		if (!GrabPoint.IsValid()) continue;
		
		const float SquaredDistance = FVector::DistSquared(GrabPoint->Execute_GetLocation(GrabPoint->_getUObject()), OwnerCharacter->GetActorLocation());
		if (MinSquaredDistance > SquaredDistance)
		{
			MinSquaredDistance = SquaredDistance;
			ReturnValue = GrabPoint;
		}
	}
	return ReturnValue;
}

bool USphereTraceOnOwner::PerformSphereTrace(TArray<FHitResult>& HitResults) const
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(RelevancyDistance);

	
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
			RelevancyDistance,
			15,
			bHit ? FColor::Green : FColor::Red,
			false,
			-1
		);
	}
#endif

	return bHit;
}

TWeakInterfacePtr<IGrabPoint> USphereTraceOnOwner::TryGetTargetGrabPoint() const
{
	TSet<TWeakInterfacePtr<IGrabPoint>> GrabbableSet = TSet<TWeakInterfacePtr<IGrabPoint>>();
	for (TWeakInterfacePtr GrabPoint : RelevantGrabPoints)
	{
		if (!GrabPoint.IsValid())
		{
			continue;
		}
			
		if (FVector::DistSquared(OwnerCharacter->GetActorLocation(), GrabPoint->Execute_GetLocation(GrabPoint->_getUObject())) <= MaxGrabDistance * MaxGrabDistance
			&& GrabPoint->Execute_CanBeGrabbed(GrabPoint->_getUObject(), OwnerCharacter))
		{
			GrabbableSet.Add(GrabPoint);
		}
	}

	TWeakInterfacePtr<IGrabPoint> MostRelevant = GetMostRelevantGrabPoint(GrabbableSet);
	if (MostRelevant != nullptr)
	{
		return MostRelevant;
	}
	return nullptr;
}
