// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Curves/CurveFloat.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GvCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_WallRun UMETA(DisplayName = "Wall Run"),
	CMOVE_RailGrind UMETA(DisplayName = "Rail Grind")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWallRunStartedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWallRunEndedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRailGrindStartedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRailGrindEndedSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CUSTOMMOVEMENTSYSTEM_API UGvCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	
	UGvCharacterMovementComponent();

	UPROPERTY(BlueprintAssignable)
	FWallRunStartedSignature OnWallRunStarted;

	UPROPERTY(BlueprintAssignable)
	FWallRunEndedSignature OnWallRunEnded;

	UPROPERTY(BlueprintAssignable)
	FRailGrindStartedSignature OnRailGrindStarted;
	
	UPROPERTY(BlueprintAssignable)
	FRailGrindEndedSignature OnRailGrindEnded;
	
private:

#pragma region WallRun
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true"))
	bool bCanWallRun = true;
	
	/**
	 * Minimum velocity to start wall running and to keep wall running.
	 * if during wall run the velocity goes below this value, the character will fall.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	float MinWallRunSpeed = 200.f;

	/**
	 * Speed of the character during wall run.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	float MaxWallRunSpeed = 800.f;

	/**
	 *  if the character exceeds this vetical speed, will not be able to wall run (Attracted to the wall).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	float MaxVerticalWallRunSpeed = 200.f;

	/**
	 * Angle in degrees to pull away from the wall.
	 * if the angle between the wall plane (wall tangent) and the acceleration is greater than this value, the character will fall.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	float WallRunPullAwayAngle = 75;

	/**
	 * Force applied to the character to push it against the wall.
	 * if the character detaches from curved walls try to increase this value.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	float WallAttractionForce = 200.f;

	/**
	 * Minimum height from the ground to wall run.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	float MinWallRunHeight = 50.f;

	/*
	 * Curve used to scale the gravity during wall run based on the acceleration direction.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	UCurveFloat* AccelerationToGravityScaleCurve;

	/**
	 * Curve used to scale the gravity during wall run based on the time.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	UCurveFloat* TimeToGravityScaleCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	float MaxWallRunDuration = 0.f;

	/**
	 * Force applied to the character along the wall normal when jumping off the wall.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	float WallJumpOffNormalForce = 300.f;

	/**
	 * Force applied to the character along the vertical axis when jumping off the wall.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	float WallJumpOffVerticalForce = 300.f;

	/**
	 * Collison channel used to detect walls.
	 * if the character have to detect all the walls, set this value to ECC_Visibility, otherwise set it to custom channel.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true", EditCondition = "bCanWallRun", EditConditionHides))
	TEnumAsByte<ECollisionChannel> WallDetectionChannel = ECollisionChannel::ECC_Visibility;

	bool bWallRunIsRight;

	float WallRunningElapsedTime = 0.f;

	bool bWallRunMaxDurationReached = false;
	
#pragma endregion

#pragma region RailGrind

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RailGrinding", meta = (AllowPrivateAccess = "true"))
	bool bCanRailGrind = true;

	/**
	 * Speed of the character during rail grind.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RailGrinding", meta = (AllowPrivateAccess = "true", EditCondition = "bCanRailGrind", EditConditionHides))
	float MaxRailGrindSpeed = 1000.f;

	/**
	 * Force applied to the character to push it against the rail before starting the grind.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RailGrinding", meta = (AllowPrivateAccess = "true", EditCondition = "bCanRailGrind", EditConditionHides, ClampMin = "0.1"))
	float RailAttractionForce = 1500.f;

	/**
	 * Offset from the character capsule base to the rail.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RailGrinding", meta = (AllowPrivateAccess = "true", EditCondition = "bCanRailGrind", EditConditionHides))
	float DistanceOffset = 0.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	USplineComponent* RailSpline;

	FVector RailGrindStartLocation;

	float GrindingDirection = 0.f;

	bool bRailGrindWasStarted = false;
	
#pragma endregion

#pragma region CoyoteJump

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CoyoteJump", meta = (AllowPrivateAccess = "true"))
	float CoyoteTime = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CoyoteJump", meta = (AllowPrivateAccess = "true"))
	bool bCoyoteJumpEnabled = false;
	
	float CoyoteTimeElapsed = 0.f;
	
#pragma endregion
	
public:

	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	virtual bool CanAttemptJump() const override;

	virtual bool DoJump(bool bReplayingMoves, float DeltaTime) override;

	virtual float GetMaxSpeed() const override;

	virtual float GetMaxBrakingDeceleration() const override;

	bool IsCustomMovementMode(const ECustomMovementMode InCustomMovementMode) const { return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode; }

#pragma region WallRun
	
	UFUNCTION(BlueprintCallable, Category = "WallRun")
	bool IsWallRunning() const { return IsCustomMovementMode(CMOVE_WallRun); }

	UFUNCTION(BlueprintCallable, Category = "WallRun")
	bool IsWallRunningRight() const { return bWallRunIsRight; }

#pragma endregion

#pragma region RailGrind

	UFUNCTION(BlueprintCallable, Category = "WallRun")
	bool IsRailGrinding() const { return IsCustomMovementMode(CMOVE_RailGrind); }

	UFUNCTION(BlueprintCallable, Category = "WallRun")
	bool TryRailGrind(USplineComponent* Spline);

#pragma endregion

#pragma region CoyoteJump

	UFUNCTION(BlueprintCallable)
	bool CanCoyoteJump() const;
	
#pragma endregion
	
protected:

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	virtual void PhysFalling(float deltaTime, int32 Iterations) override;

private:

#pragma region WallRun
	
	bool TryWallRun();
	
	void PhysWallRunning(float deltaTime, int32 Iterations);

#pragma endregion

#pragma region RailGrind
	
	void PhysRailGrinding(float deltaTime, int32 Iterations);

	void CalcGrindDirection(const float RailDist);
	
#pragma endregion
	
	float CapsuleRadius() const { return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(); }

	float CapsuleHalfHeight() const { return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(); }

	FVector CapsuleBaseLocation() const { return UpdatedComponent->GetComponentLocation() - CharacterOwner->GetActorUpVector() * CapsuleHalfHeight(); }
};
