// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WaveSystem/WaveManager.h"
#include "CompletionCondition.generated.h"


/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ENCOUNTERSYSTEM_API UCompletionCondition : public UObject
{
	GENERATED_BODY()

public:

	void Init(const UObject* WorldContextObject);

	UFUNCTION(BlueprintImplementableEvent, Category="WaveCompletionMode")
	bool IsWaveComplete(UWaveManager* InWaveManager, const UWaveData* CurrentWaveData, int32 CurrentWaveIndex, UObject* Meta);

private:
	
	UPROPERTY()
	UWorld* World = nullptr;

#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override { return true; }
# endif

	virtual UWorld* GetWorld() const override { return World; }
};
