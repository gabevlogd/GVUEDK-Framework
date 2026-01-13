// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/BaseIncrement.h"
#include "ExponentialIncrement.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UExponentialIncrement : public UBaseIncrement
{
	GENERATED_BODY()

public:

	virtual float Evaluate(const int32 WaveIndex, float BaseValue) override;

	virtual int32 Evaluate(const int32 WaveIndex, const int32 BaseValue) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float GrowthRate = 0.1f;
};
