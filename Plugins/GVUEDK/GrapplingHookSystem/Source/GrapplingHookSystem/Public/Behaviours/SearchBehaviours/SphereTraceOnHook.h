// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/GrapplingHookTargetSeekingBehavior.h"
#include "SphereTraceOnHook.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Sphere Trace On Hook"))
class GRAPPLINGHOOKSYSTEM_API USphereTraceOnHook : public UGrapplingHookTargetSeekingBehavior
{
	GENERATED_BODY()

public:
	virtual bool TickMode(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetGrabbingToleranceCurve(UCurveFloat* Curve) { GrabbingToleranceCurve = IsValid(Curve) ? Curve : GrabbingToleranceCurve; }

private:
	//return true if a target was found
	bool PerformTargetDetection(FHitResult& HitResult, const FCollisionQueryParams& CollisionParams, const FVector& HookLocation);
	//return true if an obstacle was found
	bool PerformObstacleDetection(FHitResult& HitResult, const FCollisionQueryParams& CollisionParams, const FVector& HookLocation);

	float GetGrabbingTolerance();
	
protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> GrabbableTraceChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* GrabbingToleranceCurve;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float LinearGrabbingTolerance = 50.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> HookCollisionTraceChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float ObstacleTolerance = 10.f;
};
