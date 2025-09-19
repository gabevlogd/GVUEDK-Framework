// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Behaviours/Base/GrapplingHookBehaviourBase.h"
#include "GHMovementBehaviourBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew)
class GRAPPLINGHOOKSYSTEM_API UGHMovementBehaviourBase : public UGrapplingHookBehaviourBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual bool StartHooking(){ return false; };
	UFUNCTION(BlueprintCallable)
	virtual void StopHooking(){};
	
	UFUNCTION(BlueprintCallable)
	virtual float GetElapsedTime() const { return StartDelayTimer; }
	UFUNCTION(BlueprintCallable)
	virtual float GetStartDelay() const { return StartDelay; }

protected:
	virtual bool TryCalculateMotionData() { return true; }

protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float StartDelay;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bApplyMomentumDuringHookThrow;

	EMovementMode PreviousMovementMode;
	float StartDelayTimer;
	
	bool bIsHooking;
	bool bMotionDataCalculated;
};
