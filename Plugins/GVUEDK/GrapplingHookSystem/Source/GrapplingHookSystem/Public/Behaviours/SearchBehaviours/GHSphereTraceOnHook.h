// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/GHSearchBehaviourBase.h"
#include "GHSphereTraceOnHook.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Sphere Trace On Hook"))
class GRAPPLINGHOOKSYSTEM_API UGHSphereTraceOnHook : public UGHSearchBehaviourBase
{
	GENERATED_BODY()

public:
	virtual bool TickMode(float DeltaTime) override;

private:
	//return true if a target was found
	bool PerformTargetDetection(FHitResult& HitResult, const FCollisionQueryParams& CollisionParams, const FVector& HookLocation);
	//return true if an obstacle was found
	bool PerformObstacleDetection(FHitResult& HitResult, const FCollisionQueryParams& CollisionParams, const FVector& HookLocation);

	float GetGrabbingTollerane();
	
protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> GrabbableTraceChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* GrabbingTolleraneCurve;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float LinearGrabbingTollerane = 50.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> HookCollsionTraceChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float ObstacleTollerane = 10.f;
};
