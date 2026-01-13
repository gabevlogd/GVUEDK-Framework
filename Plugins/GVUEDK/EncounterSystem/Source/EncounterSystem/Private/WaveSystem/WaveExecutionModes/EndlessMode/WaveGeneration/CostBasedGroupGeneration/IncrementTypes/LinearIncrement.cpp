// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/EndlessMode/WaveGeneration/CostBasedGroupGeneration/IncrementTypes/LinearIncrement.h"

float ULinearIncrement::Evaluate(const int32 WaveIndex, const float BaseValue)
{
	return BaseValue + (IncrementPerWave * WaveIndex);
}

int32 ULinearIncrement::Evaluate(const int32 WaveIndex, const int32 BaseValue)
{
	return BaseValue + (IncrementPerWave * WaveIndex);
}
