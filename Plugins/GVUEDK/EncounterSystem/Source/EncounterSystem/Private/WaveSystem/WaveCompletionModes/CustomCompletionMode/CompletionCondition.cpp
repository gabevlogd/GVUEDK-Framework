// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveCompletionModes/CustomCompletionMode/CompletionCondition.h"

void UCompletionCondition::Init(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		World = WorldContextObject->GetWorld();
	}
}
