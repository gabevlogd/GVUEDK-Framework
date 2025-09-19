// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "DebugInteractableActor.generated.h"

UCLASS()
class INTERACTIONSYSTEM_API ADebugInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ADebugInteractableActor();
	
	//virtual void Interact_Implementation() override;
};
