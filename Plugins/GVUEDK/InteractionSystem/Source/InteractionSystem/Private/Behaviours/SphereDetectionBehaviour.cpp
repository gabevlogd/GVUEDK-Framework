// Copyright Villains, Inc. All Rights Reserved.


#include "Behaviours/SphereDetectionBehaviour.h"

#include <string>

#include "Engine/OverlapResult.h"
#include "Interfaces/Interactable.h"
#include "Kismet/GameplayStatics.h"


bool USphereDetectionBehaviour::TryGetInteractable(TScriptInterface<IInteractable>& OutInteractable) const
{
	OutInteractable = nullptr;
	TArray<FOverlapResult> Overlaps;
	//Check if there are any actors that match the trace channel and are within the max detection distance
	if (GetWorld()->OverlapMultiByChannel(Overlaps, Owner->GetActorLocation(), FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(MaxDetectionDistance)))
	{
		//Check if the overlapped actors are interactable
		TArray<TScriptInterface<IInteractable>> Interactables = TArray<TScriptInterface<IInteractable>>();
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UInteractable>())
			{
				if (IInteractable::Execute_CanBeInteracted(Overlap.GetActor(), Owner))
				{
					Interactables.Add(Overlap.GetActor());
				}
			}
		}
		
		//If there are more than one interactable, get the most relevant
		if (Interactables.Num() > 1)
		{
			OutInteractable = GetMostRelevantInteractable(Interactables);
		}
		//If there is only one interactable, return it
		if (Interactables.Num() == 1)
		{
			OutInteractable = Interactables[0];
		}
		
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		if (bShowDebug)
		{
			DrawDebugSphere(GetWorld(), Owner->GetActorLocation(), MaxDetectionDistance, 12, FColor::Blue, false, -1.f);
			if (OutInteractable.GetObject() != nullptr)
			{
				DrawDebugSphere(GetWorld(), OutInteractable->Execute_GetInteractableLocation(OutInteractable.GetObject()), InteractableSphereRadius, 12, FColor::Red, false, -1.f);
			}
		}
#endif

		return OutInteractable.GetObject() != nullptr;
	}
	return false;
}


TScriptInterface<IInteractable> USphereDetectionBehaviour::GetMostRelevantInteractable(TArray<TScriptInterface<IInteractable>> Interactables) const
{
	TScriptInterface<IInteractable> MostRelevantInteractable = nullptr;
	float AngleCoincidence = MaxDetectionAngle;
	const FVector CameraForward = Cast<APawn>(Owner)->GetControlRotation().Vector();
	
	for (auto Element : Interactables)
	{
		FVector Direction = (Element->Execute_GetInteractableLocation(Element.GetObject()) - GetOwnerCameraLocation()).GetSafeNormal();
		float Dot = FVector::DotProduct(Direction, CameraForward);
		if (Dot >= AngleCoincidence)
		{
			AngleCoincidence = Dot;
			MostRelevantInteractable = Element;
		}
	}
	return MostRelevantInteractable;
}


FVector USphereDetectionBehaviour::GetOwnerCameraLocation() const
{
	return UGameplayStatics::GetPlayerCameraManager(Owner, 0)->GetCameraCacheView().Location;
}
