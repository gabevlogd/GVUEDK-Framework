// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/EnvironmentTracingComponent.h"
#include "GroundCheckComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GVUEDK_API UGroundCheckComponent : public UEnvironmentTracingComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = true, AllowPrivateAccess = "true"))
	FVector StartTraceLocation;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float MaxGroundDistance = 3000.0f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float SphereCastRadius = 50.0f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> GroundTraceChannel = ECC_Visibility;

public:
	UGroundCheckComponent();
	FORCEINLINE float GetMaxGroundDistance() const { return MaxGroundDistance; }
	
	UFUNCTION(BlueprintCallable)
	float CheckGroundDistance() const;

	UFUNCTION(BlueprintCallable)
	bool IsGrounded(const float Tolerance) const;

};
