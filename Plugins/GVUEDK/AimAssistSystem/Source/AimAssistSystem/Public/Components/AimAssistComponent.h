// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AimAssistComponent.generated.h"

class UCurveVector;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewTargetDetected, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetLost, AActor*, Target);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AIMASSISTSYSTEM_API UAimAssistComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FNewTargetDetected OnNewTargetDetected;
	
	UPROPERTY(BlueprintAssignable)
	FTargetLost OnTargetLost;

private:
	/**
	 * The maximum distance to detect targets for aim assist
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist", meta = (AllowPrivateAccess = "true"))
	float MaxDetectionDistance;

	/**
	 * The minimum distance to detect targets for aim assist
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist", meta = (AllowPrivateAccess = "true"))
	float MinDetectionDistance;

	/**
	 * The radius of the sphere sweeped to detect targets
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist", meta = (AllowPrivateAccess = "true", ClampMin = 0.f))
	float DetectionRadius;

	/**
	 * The angle in degrees to consider a target as pointed
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist", meta = (AllowPrivateAccess = "true", ClampMax = 90.f, ClampMin = 0.f))
	float AimAssistMinAngle;

	/**
	 * The force to adjust the aim rotation over TIME (if no curve is provided AdjustAimForce will be used instead)
	 * Horizontal axis represents the time in seconds, Vertical axis represents the force to adjust the aim. \n
	 * X: the force to adjust the aim when a target is detected (transition in) \n
	 * Y: the force to adjust the aim when a target is lost (transition out) \n
	 * Z: not used \n
	 * NOTE: max value for X and Y must be the same.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist", meta = (AllowPrivateAccess = "true"))
	UCurveVector* AdjustAimCurve;

	/**
	 * The force to adjust the aim rotation
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist", meta = (AllowPrivateAccess = "true"))
	float AdjustAimForce;

	/**
	 *  The object channel off actors to detect as targets
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> TargetsObjectChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist", meta = (AllowPrivateAccess = "true"))
	FName BoneToAim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist", meta = (AllowPrivateAccess = "true"))
	bool bShowDebug = false;

	UPROPERTY()
	AController* Controller;
	
	UPROPERTY()
	APlayerCameraManager* CameraManager;
	
	UPROPERTY()
	AActor* CurrentTarget;
	
	UPROPERTY()
	AActor* PreviousTarget;

	UPROPERTY()
	AActor* LastValidTarget;

	/**
     * The elapsed time since the current target was detected
     */
	float TargetDetectionTime;
	
	/**
     * The elapsed time since the last valid target was lost
     */
	float TargetLostTime;
	
	float LastAjustAimForceOut;
	
	bool bHasTarget;

public:
	
	UAimAssistComponent();

	UFUNCTION(BlueprintCallable, Category = "Aim Assist")
	void EnableAimAssist();
	
	UFUNCTION(BlueprintCallable, Category = "Aim Assist")
	void DisableAimAssist();

private:
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateCurrentTarget(const float DeltaTime);

	void UpdatePreviousTarget(const float DeltaTime);
	
	void AdjustAim(const float DeltaTime, const AActor* TargetToAim, const float AimAdjustmentForce) const;
	
	void AdjustAimSmooth(const float DeltaTime);

	AActor* GetMostPointedTarget(const TArray<FHitResult>& Targets, const FVector& AimDirection, const FVector& EyeLocation) const;

	float GetAdjustAimForceIn(const float Time) const;

	float GetAdjustAimForceOut(const float Time) const;
};
