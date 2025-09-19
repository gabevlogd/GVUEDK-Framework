// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnvironmentTracingComponent.generated.h"


UCLASS(Abstract, ClassGroup=(Custom))
class GVUEDK_API UEnvironmentTracingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	bool bInitialized;

private:
	UPROPERTY()
	UWorld* World;

	UPROPERTY(EditAnywhere,
		meta = (AllowPrivateAccess = "true", ToolTip = "Ignore the owner of this component when tracing"))
	bool bIgnoreSelf = true;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ToolTip = "Show debug information"))
	bool bShowDebug = false;

	FCollisionQueryParams QueryParams;

public:
	UEnvironmentTracingComponent();
	virtual void BeginPlay() override;

protected:
	bool PerformLineTrace(const FVector& Start, const FVector& End, FHitResult& OutHitResult,
	                      ECollisionChannel TraceChannel = ECC_Visibility) const;
	bool PerformSphereTrace(const FVector& Start, const FVector& End, float Radius, FHitResult& OutHitResult,
	                        ECollisionChannel TraceChannel = ECC_Visibility) const;
};
