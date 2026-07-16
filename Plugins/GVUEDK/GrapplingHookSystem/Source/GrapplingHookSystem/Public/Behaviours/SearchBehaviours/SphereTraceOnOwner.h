// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/GrapplingHookTargetSeekingBehavior.h"
#include "SphereTraceOnOwner.generated.h"

inline uint32 GetTypeHash(const TWeakInterfacePtr<IGrabPoint>& WeakPtr)
{
	return GetTypeHash(WeakPtr.GetWeakObjectPtr());
}

/**
 * 
 */
UCLASS(meta = (DisplayName = "Sphere Trace On Owner"))
class GRAPPLINGHOOKSYSTEM_API USphereTraceOnOwner : public UGrapplingHookTargetSeekingBehavior
{
	GENERATED_BODY()

public:
	USphereTraceOnOwner();

	virtual void Initialize(UGrapplingHookComponent* InGrapplingHookComponent) override;
	virtual bool TickMode(float DeltaTime) override;
	virtual void ExitMode() override;

	UFUNCTION(BlueprintCallable)
	float GetMaxDistance() const { return MaxGrabDistance; }
	UFUNCTION(BlueprintCallable)
	float GetMinDistance() const { return MinGrabDistance; }

private:
	bool LookForGrabPoints(TSet<TWeakInterfacePtr<IGrabPoint>>& OutGrabPoints) const;
	TWeakInterfacePtr<IGrabPoint> GetMostRelevantGrabPoint(TSet<TWeakInterfacePtr<IGrabPoint>> OutGrabPoints) const;
	TWeakInterfacePtr<IGrabPoint> TryGetTargetGrabPoint() const;
	bool PerformSphereTrace(TArray<FHitResult>& HitResults) const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> GrabPointsChannel;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true" , ClampMin = "1.0"))
	float RelevancyDistance;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float MaxGrabDistance;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float MinGrabDistance;
	
	
	TSet<TWeakInterfacePtr<IGrabPoint>> RelevantGrabPoints;
	TWeakInterfacePtr<IGrabPoint> LastTargetGrabPoint;
	
};
