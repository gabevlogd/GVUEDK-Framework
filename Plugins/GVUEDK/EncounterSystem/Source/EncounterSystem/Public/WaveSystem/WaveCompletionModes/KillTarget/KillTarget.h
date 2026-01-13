// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WaveSystem/WaveCompletionModes/Base/WaveCompletionMode.h"
#include "KillTarget.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UKillTarget : public UWaveCompletionMode
{
	GENERATED_BODY()

public:

	virtual bool Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager) override;

	virtual bool IsWaveComplete() override;
	
	virtual void BeginDestroy() override;

private:

	UFUNCTION()
	void RegisterTarget(AActor* Target);

	UFUNCTION()
	void UnregisterTarget(AActor* Target);

	bool bTargetSpawned = false;

	UPROPERTY()
	TArray<AActor*> ActiveTargets;
};
