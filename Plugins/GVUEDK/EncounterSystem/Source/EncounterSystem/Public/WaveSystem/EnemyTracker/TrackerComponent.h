// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrackerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrackedEnemyDestroyed, AActor*, DestroyedEnemy);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ENCOUNTERSYSTEM_API UTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	static FOnTrackedEnemyDestroyed* OnTrackedEnemyDestroyed;
	
	UTrackerComponent();

	UFUNCTION(BlueprintCallable, Category="TrackerComponent")
	void UnregisterActor() const;
};
