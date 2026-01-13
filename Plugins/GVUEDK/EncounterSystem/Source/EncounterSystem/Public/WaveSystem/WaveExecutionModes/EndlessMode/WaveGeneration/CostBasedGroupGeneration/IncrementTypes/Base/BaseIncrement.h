// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BaseIncrement.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class ENCOUNTERSYSTEM_API UBaseIncrement : public UObject
{
	GENERATED_BODY()

public:

	
	virtual float Evaluate(const int32 WaveIndex, const float BaseValue) PURE_VIRTUAL(UBudgetIncrement::GetBudgetForWave, return BaseValue;);
	virtual int32 Evaluate(const int32 WaveIndex, const int32 BaseValue) PURE_VIRTUAL(UBudgetIncrement::GetBudgetForWave, return BaseValue;);
};
