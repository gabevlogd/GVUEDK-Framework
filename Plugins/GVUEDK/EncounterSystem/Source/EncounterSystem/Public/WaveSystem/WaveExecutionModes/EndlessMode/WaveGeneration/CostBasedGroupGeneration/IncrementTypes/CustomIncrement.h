// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/BaseIncrement.h"
#include "CustomIncrement.generated.h"

/**
 * 
 */
UCLASS(Abstract, meta=(HideCategories="Hidden"))
class ENCOUNTERSYSTEM_API UCustomIncrement : public UBaseIncrement
{
	GENERATED_BODY()

public:

	virtual float Evaluate(const int32 WaveIndex, float BaseValue) override;

	virtual int32 Evaluate(const int32 WaveIndex, const int32 BaseValue) override;

	UFUNCTION(BlueprintImplementableEvent)
	float FloatEvaluate(const int32 WaveIndex, const float BaseValue);
	
	UFUNCTION(BlueprintImplementableEvent)
	int32 IntEvaluate(const int32 WaveIndex, const int32 BaseValue);

private:

	UPROPERTY()
	UWorld* World = nullptr;

#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override { return true; }
# endif

	virtual UWorld* GetWorld() const override { return World; }
};
