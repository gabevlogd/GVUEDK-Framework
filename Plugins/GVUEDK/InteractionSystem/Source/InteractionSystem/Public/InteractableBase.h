// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "InteractableBase.generated.h"

UCLASS()
class INTERACTIONSYSTEM_API AInteractableBase : public AActor, public IInteractable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bObstacleInBetweenAllowed = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	mutable bool bCanBeInteracted = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bSingleUse = false;
	

public:
	AInteractableBase();
	virtual void Interact_Implementation() override;
	virtual bool CanBeInteracted_Implementation(AActor* Caller) const override;
	virtual FVector GetInteractableLocation_Implementation() const override;

private:
	void CheckSingleUse() const;
};
