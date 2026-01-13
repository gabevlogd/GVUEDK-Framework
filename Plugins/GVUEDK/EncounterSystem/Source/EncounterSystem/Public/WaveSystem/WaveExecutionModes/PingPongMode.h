// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LoopingMode.h"
#include "PingPongMode.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UPingPongMode : public ULoopingMode
{
	GENERATED_BODY()

public:

	virtual int32 GetNextWaveIndex() override;

private:

	static bool bReversing;
};
