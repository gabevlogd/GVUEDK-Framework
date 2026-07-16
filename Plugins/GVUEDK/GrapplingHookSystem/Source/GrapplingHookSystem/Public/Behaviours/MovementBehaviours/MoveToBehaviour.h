// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/GrapplingHookMovementBehaviour.h"
#include "MoveToBehaviour.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "GHMoveToBehaviour"))
class GRAPPLINGHOOKSYSTEM_API UMoveToBehaviour : public UGrapplingHookMovementBehaviour
{
	GENERATED_BODY()

public:
	UMoveToBehaviour();
	
	virtual bool StartHooking() override;
	virtual void StopHooking() override;
	virtual bool TickMode(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	float GetTotalRelativeDistance() const { return TotalRelativeSquaredDistance; }
	UFUNCTION(BlueprintCallable)
	FVector GetCurrentDirection() const { return bIsHooking ? GetDirection() : FVector::ZeroVector; }

protected:
	virtual bool TryCalculateMotionData() override;
	
private:
	void PerformMotion(float DeltaTime);
	void OrientRotationToMovement(float DeltaTime);
	float GetSpeed();
	FVector GetEndLocation() const;
	FVector GetDirection() const;

	/** Check if there are any obstacles between the thrower and the target grab point
	 *  @return true if there are obstacles, false otherwise
	 */
	bool CheckObstacles() const;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* SpeedCurve;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ToolTip = "Used only if no SpeedCurve provided"))
	float LinearSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bOrientRotationToMovement;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> ObstaclesTraceChannel;
	
	FVector StartRelativeLocation;
	
	float TotalRelativeSquaredDistance;
	
};
