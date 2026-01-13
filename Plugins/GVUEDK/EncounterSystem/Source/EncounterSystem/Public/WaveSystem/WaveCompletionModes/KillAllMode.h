// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/WaveCompletionMode.h"
#include "KillAllMode.generated.h"
/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UKillAllMode : public UWaveCompletionMode
{
	GENERATED_BODY()

public:

	virtual bool Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager) override;

	virtual bool IsWaveComplete() override;

private:
	
	/**
	 *  If true, "All" means all enemies in the entire level, otherwise it means all enemies spawned by this wave only
	 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bGlobalKill = true;
};
