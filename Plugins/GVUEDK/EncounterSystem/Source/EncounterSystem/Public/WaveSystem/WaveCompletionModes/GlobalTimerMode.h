// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TimerMode.h"
#include "Base/WaveCompletionMode.h"
#include "GlobalTimerMode.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UGlobalTimerMode : public UTimerMode
{
	GENERATED_BODY()

public:

	virtual bool Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager) override;

	virtual bool IsWaveComplete() override;

	virtual float GetWaveRemainingTime() const override;

	virtual float GetAllWaveRemainingTime() const;

	virtual bool HasNextWave() const override;

protected:

	UFUNCTION()
	virtual void CurrentWaveTimerCompleted();

	virtual void TimerCompleted() override;

	virtual void Deinitialize(const UWaveData* WaveData, const int32 WaveIndex) override;

	UFUNCTION()
	virtual void InvalidateGlobalTimer(const UWaveData* WaveData, const int32 WaveIndex);

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float CurrentWaveCompletionTime = -1.f;

	bool bCurrentWaveTimerCompleted = false;

	FTimerHandle CurrentWaveTimerHandle;

	static FTimerHandle GlobalTimerHandle;
};
