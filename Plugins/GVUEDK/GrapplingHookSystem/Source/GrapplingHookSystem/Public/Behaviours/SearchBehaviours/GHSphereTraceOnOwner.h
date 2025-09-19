// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/GHSearchBehaviourBase.h"
#include "GHSphereTraceOnOwner.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Sphere Trace On Owner"))
class GRAPPLINGHOOKSYSTEM_API UGHSphereTraceOnOwner : public UGHSearchBehaviourBase
{
	GENERATED_BODY()

public:
	UGHSphereTraceOnOwner();

	virtual void Initialize(UGrapplingHookComponent* InGrapplingHookComponent) override;
	virtual bool TickMode(float DeltaTime) override;
	virtual void ExitMode() override;

	UFUNCTION(BlueprintCallable)
	float GetMaxDistance() const { return MaxDistance; }
	UFUNCTION(BlueprintCallable)
	float GetMinDistance() const { return MinDistance; }

private:
	bool LookForGrabPoints(TSet<IGrabPoint*>& OutGrabPoints) const;
	IGrabPoint* GetMostRelevantGrabPoint(TSet<IGrabPoint*>& ValidGrabPoints) const;
	bool PerformSphereTrace(TArray<FHitResult>& HitResults) const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> GrabPointsChannel;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float MaxDistance;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float MinDistance;
	
	
	TSet<IGrabPoint*> InRangeGrabPoints;
	IGrabPoint* LastTargetGrabPoint;
	
};
