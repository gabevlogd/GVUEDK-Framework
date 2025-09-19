// Copyright Villains, Inc. All Rights Reserved.


#include "Components/CompassTargetComponent.h"

#include "Utility/UCompassUtility.h"


UCompassTargetComponent::UCompassTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCompassTargetComponent::BeginPlay()
{
	if (bAutoAddToCompass)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindLambda([this]()
		{
			UCompassUtility::AddTarget(this);
		});
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 0.5f, false);
	}
	
	Super::BeginPlay();
	
}

