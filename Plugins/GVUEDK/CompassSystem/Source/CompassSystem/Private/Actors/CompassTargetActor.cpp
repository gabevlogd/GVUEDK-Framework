// Copyright Villains, Inc. All Rights Reserved.


#include "Actors/CompassTargetActor.h"


ACompassTargetActor::ACompassTargetActor()
{
	CompassTargetComponent = CreateDefaultSubobject<UCompassTargetComponent>(TEXT("CompassTargetComponent"));
}
