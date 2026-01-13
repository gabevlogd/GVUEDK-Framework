// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/BaseIncrement.h"
#include "LinearIncrement.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API ULinearIncrement : public UBaseIncrement
{
	GENERATED_BODY()

public:

	virtual float Evaluate(const int32 WaveIndex, float BaseValue) override;

	virtual int32 Evaluate(const int32 WaveIndex, const int32 BaseValue) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1"))
	int32 IncrementPerWave = 5;
};
