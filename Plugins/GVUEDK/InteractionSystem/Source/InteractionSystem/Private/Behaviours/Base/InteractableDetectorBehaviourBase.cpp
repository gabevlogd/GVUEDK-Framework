// Copyright Villains, Inc. All Rights Reserved.


#include "Behaviours/Base/InteractableDetectorBehaviourBase.h"

UInteractableDetectorBehaviourBase::UInteractableDetectorBehaviourBase() :
Owner(nullptr)
{
	
}

void UInteractableDetectorBehaviourBase::Initialize(AActor* InOwner)
{
	Owner = InOwner;
}
