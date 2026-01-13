// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/WaveCompletionMode.h"
#include "TimerMode.generated.h"

class UWaveData;
/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UTimerMode : public UWaveCompletionMode
{
	GENERATED_BODY()

public:

	virtual bool Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager) override;
	
	virtual bool IsWaveComplete() override;

	virtual float GetWaveRemainingTime() const;

protected:

	virtual void BeginDestroy() override;

	virtual void TimerCompleted();
	
	UFUNCTION()
	virtual void Deinitialize(const UWaveData* WaveData, const int32 WaveIndex);

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true", ClampMin = "0.1", UIMin = "0.1"))
	float TimerDuration = 10.f;
	
	FTimerHandle TimerHandle;

	bool bTimerCompleted = false;
};
