// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/EndlessMode/WaveGeneration/CostBasedGroupGeneration/IncrementTypes/ExponentialIncrement.h"

float UExponentialIncrement::Evaluate(const int32 WaveIndex, const float BaseValue)
{
	return BaseValue * FMath::Pow(EULERS_NUMBER, GrowthRate * WaveIndex); 
}

int32 UExponentialIncrement::Evaluate(const int32 WaveIndex, const int32 BaseValue)
{
	return FMath::RoundToInt32(BaseValue * FMath::Pow(EULERS_NUMBER, GrowthRate * WaveIndex)); 
}
