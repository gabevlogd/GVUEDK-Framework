// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/EndlessMode/WaveGeneration/CostBasedGroupGeneration/IncrementTypes/CustomIncrement.h"

float UCustomIncrement::Evaluate(const int32 WaveIndex, const float BaseValue)
{
	return FloatEvaluate(WaveIndex, BaseValue);
}

int32 UCustomIncrement::Evaluate(const int32 WaveIndex, const int32 BaseValue)
{
	return IntEvaluate(WaveIndex, BaseValue);
}
