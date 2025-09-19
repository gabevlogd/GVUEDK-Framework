// Copyright Villains, Inc. All Rights Reserved.


#include "Components/InteractableDetectorComponent.h"

#include "Behaviours/Base/InteractableDetectorBehaviourBase.h"

UInteractableDetectorComponent::UInteractableDetectorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractableDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(DetectorBehaviour))
	{
		UE_LOG(LogTemp, Error, TEXT("%s Behaviour is not valid"), *GetName());
		return;
	}
	DetectorBehaviour->Initialize(GetOwner());
}

void UInteractableDetectorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CheckInteractablesInRange();
}

bool UInteractableDetectorComponent::TryGetInteractable(TScriptInterface<IInteractable>& OutInteractable) const
{
	return DetectorBehaviour->TryGetInteractable(OutInteractable);
}

void UInteractableDetectorComponent::CheckInteractablesInRange()
{
	TScriptInterface<IInteractable> OutInteractable = nullptr;
	if (TryGetInteractable(OutInteractable))
	{
		if (OutInteractable.GetObject() != LastDetectedInteractable.GetObject())
		{
			if (IsValid(LastDetectedInteractable.GetObject()))
				IInteractable::Execute_Unhighlight(LastDetectedInteractable.GetObject());
			
			LastDetectedInteractable = OutInteractable;
			IInteractable::Execute_Highlight(OutInteractable.GetObject());
			
			OnInteractableDetected.Broadcast(OutInteractable);
		}
	}
	else if (LastDetectedInteractable.GetObject() != nullptr)
	{
		IInteractable::Execute_Unhighlight(LastDetectedInteractable.GetObject());
		LastDetectedInteractable = nullptr;
		OnInteractableLost.Broadcast();
	}
}




