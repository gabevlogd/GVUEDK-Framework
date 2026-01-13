// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/ConditionalMode/NextWaveCondition.h"

void UNextWaveCondition::Init(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		World = WorldContextObject->GetWorld();
		Init();
	}
}
