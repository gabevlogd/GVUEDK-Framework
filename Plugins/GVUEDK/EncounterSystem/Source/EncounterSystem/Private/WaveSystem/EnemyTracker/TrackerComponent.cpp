// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/EnemyTracker/TrackerComponent.h"


FOnTrackedEnemyDestroyed* UTrackerComponent::OnTrackedEnemyDestroyed = new FOnTrackedEnemyDestroyed();

UTrackerComponent::UTrackerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTrackerComponent::UnregisterActor() const 
{
	OnTrackedEnemyDestroyed->Broadcast(GetOwner());
}


