// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/CompassTargetComponent.h"
#include "UObject/Object.h"
#include "CompassTargetActor.generated.h"

/**
 * 
 */
UCLASS()
class COMPASSSYSTEM_API ACompassTargetActor : public AActor
{
	GENERATED_BODY()

public:

	ACompassTargetActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCompassTargetComponent* CompassTargetComponent;
};
