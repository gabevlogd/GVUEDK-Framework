// Copyright Villains, Inc. All Rights Reserved.


#include "InteractableBase.h"


// Sets default values
AInteractableBase::AInteractableBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AInteractableBase::Interact_Implementation()
{
	CheckSingleUse();
}

bool AInteractableBase::CanBeInteracted_Implementation(AActor* Caller) const
{
	if (!bCanBeInteracted) return false;
	if (bObstacleInBetweenAllowed) return true;
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(Caller);
	GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), Caller->GetActorLocation(), ECC_Visibility, CollisionParams);
	return !HitResult.bBlockingHit;
}

FVector AInteractableBase::GetInteractableLocation_Implementation() const
{
	return GetActorLocation();
}

void AInteractableBase::CheckSingleUse() const
{
	if (bSingleUse)
	{
		bCanBeInteracted = false;
	}
}
