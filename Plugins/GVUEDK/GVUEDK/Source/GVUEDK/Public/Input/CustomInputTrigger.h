// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "CustomInputTrigger.generated.h"

/**
 * 
 */
UCLASS()
class GVUEDK_API UCustomInputTrigger : public UInputTrigger
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float InputElapsedTimeLimit = 0.2f;
	float InputElapsedTime = 0.0f;
};
