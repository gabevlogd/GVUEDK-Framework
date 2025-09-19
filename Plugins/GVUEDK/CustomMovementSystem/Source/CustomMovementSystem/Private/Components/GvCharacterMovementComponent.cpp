// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GvCharacterMovementComponent.h"


// Sets default values for this component's properties
UGvCharacterMovementComponent::UGvCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGvCharacterMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	Super::ProcessLanded(Hit, remainingTime, Iterations);
	bWallRunMaxDurationReached = false;
	CoyoteTimeElapsed = 0.f;
}

void UGvCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (IsFalling())
	{
		TryWallRun();
	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

bool UGvCharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump() || IsWallRunning() || IsRailGrinding() || CanCoyoteJump();
}

bool UGvCharacterMovementComponent::DoJump(bool bReplayingMoves, float DeltaTime)
{
	const bool bWasWallRunning = IsWallRunning();
	const bool bWasRailGrinding = IsRailGrinding();
	if (Super::DoJump(bReplayingMoves, DeltaTime))
	{

		CoyoteTimeElapsed = CoyoteTime + 1.f; // Reset Coyote time to prevent double jump
		
		// If we are wall running, we need to apply a force to the character to push them away from the wall
		if (bWasWallRunning)
		{
			const FVector Start = UpdatedComponent->GetComponentLocation();
			const FVector TraceLenght = UpdatedComponent->GetRightVector() * CapsuleRadius() * 2.f;
			const FVector End = bWallRunIsRight ? Start + TraceLenght : Start - TraceLenght;
			
			FCollisionQueryParams Params = FCollisionQueryParams(FName(TEXT("WallRun")), true, CharacterOwner);
			TArray<AActor*> ChildActors;
			CharacterOwner->GetAllChildActors(ChildActors);
			Params.AddIgnoredActors(ChildActors);
			
			FHitResult WallHit;
			GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
			Velocity += WallHit.Normal * WallJumpOffNormalForce;
			Velocity.Z = WallJumpOffVerticalForce;

			OnWallRunEnded.Broadcast();
		}
		else if (bWasRailGrinding)
		{
			RailSpline = nullptr;
			OnRailGrindEnded.Broadcast();
		}
		return true;
	}
	return false;
}

float UGvCharacterMovementComponent::GetMaxSpeed() const
{
	if (MovementMode != MOVE_Custom)
	{
		return Super::GetMaxSpeed();
	}

	if (CustomMovementMode == CMOVE_WallRun)
	{
		return MaxWallRunSpeed;
	}

	if (CustomMovementMode == CMOVE_RailGrind)
	{
		return MaxRailGrindSpeed;
	}

	UE_LOG(LogTemp, Warning, TEXT("Invalid Custom Movement Mode: %d"), CustomMovementMode);
	return -1.f;
}

float UGvCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (MovementMode != MOVE_Custom)
	{
		return Super::GetMaxBrakingDeceleration();
	}

	if (CustomMovementMode == CMOVE_WallRun || CustomMovementMode == CMOVE_RailGrind)
	{
		return 0.f;
	}

	UE_LOG(LogTemp, Warning, TEXT("Invalid Custom Movement Mode: %d"), CustomMovementMode);
	return -1.f;
	
}

void UGvCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	if (CustomMovementMode == CMOVE_WallRun)
	{
		PhysWallRunning(deltaTime, Iterations);
		return;
	}

	if (CustomMovementMode == CMOVE_RailGrind)
	{
		PhysRailGrinding(deltaTime, Iterations);
		return;
	}
}

void UGvCharacterMovementComponent::PhysFalling(float deltaTime, int32 Iterations)
{
	Super::PhysFalling(deltaTime, Iterations);

	if (bCoyoteJumpEnabled)
	{
		CoyoteTimeElapsed += deltaTime;
	}
}

bool UGvCharacterMovementComponent::TryWallRun()
{
	if (!bCanWallRun) return false;
	if (bWallRunMaxDurationReached) return false;
	if (!IsFalling()) return false;
	if (Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2)) return false;
	if (Velocity.Z < -MaxVerticalWallRunSpeed) return false; // Prevent wall run if falling too fast
	
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector TraceLenght = UpdatedComponent->GetRightVector() * CapsuleRadius() * 2.f;
	FVector LeftEnd = Start - TraceLenght;
	FVector RightEnd = Start + TraceLenght;
	
	FCollisionQueryParams Params = FCollisionQueryParams(FName(TEXT("WallRun")), true, CharacterOwner);
	TArray<AActor*> ChildActors;
	CharacterOwner->GetAllChildActors(ChildActors);
	Params.AddIgnoredActors(ChildActors);
	
	FHitResult FloorHit;
	// if hit the floor, character is too near the ground to wall run
	if (GetWorld()->LineTraceSingleByProfile(FloorHit, Start, Start + FVector::DownVector * (CapsuleHalfHeight() + MinWallRunHeight), "BlockAll", Params))
	{
		return false;
	}

	FHitResult WallHit;
	// Check wall on the left
	GetWorld()->LineTraceSingleByChannel(WallHit, Start, LeftEnd, WallDetectionChannel, Params);
	// if we hit a wall and if velocity is into the wall or at least parallel to it
	if (WallHit.IsValidBlockingHit() && (Velocity | WallHit.Normal) <= 0) 
	{
		bWallRunIsRight = false;
	}
	else
	{
		// Check wall on the right
		GetWorld()->LineTraceSingleByChannel(WallHit, Start, RightEnd, WallDetectionChannel, Params);
		if (WallHit.IsValidBlockingHit() && (Velocity | WallHit.Normal) <= 0)
		{
			bWallRunIsRight = true;
		}
		else
		{
			return false;
		}
	}

	// Check if the wall is nearly vertical
	if (!FMath::IsNearlyZero(WallHit.Normal | FVector::UpVector, 0.01f))
	{
		return false;
	}

	// Project the velocity along the wall tangent
	FVector ProjectedVelocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
	if (ProjectedVelocity.SizeSquared2D() < pow(MinWallRunSpeed, 2)) return false;
	
	// if all checks passed, we can start wall running
	Velocity = ProjectedVelocity;
	Velocity.Z = FMath::Clamp(Velocity.Z, 0.f, MaxVerticalWallRunSpeed);
	WallRunningElapsedTime = 0.f;
	bWallRunMaxDurationReached = false;
	SetMovementMode(MOVE_Custom, CMOVE_WallRun);
	OnWallRunStarted.Broadcast();
	return true;
}

void UGvCharacterMovementComponent::PhysWallRunning(float deltaTime, int32 Iterations)
{
	// All line marked by empty comments "/**/" are to follow the same logic as PhysWalking

	/**/
	if (deltaTime < MIN_TICK_TIME) 
	{
		return;
	}
	/**/
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}
	
	bJustTeleported = false; /**/
	float RemainingTime = deltaTime; /**/

	auto StopWallRun = [&]()
	{
		SetMovementMode(MOVE_Falling);
		StartNewPhysics(RemainingTime, Iterations);
		OnWallRunEnded.Broadcast();
	};
	
	/**/
	while ( (RemainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)) )
	{
		Iterations++; /**/
		bJustTeleported = false; /**/
		const float timeTick = GetSimulationTimeStep(RemainingTime, Iterations); /**/
		RemainingTime -= timeTick; /**/
		const FVector OldLocation = UpdatedComponent->GetComponentLocation(); /**/
		
		FVector Start = UpdatedComponent->GetComponentLocation();
		FVector TraceLenght = UpdatedComponent->GetRightVector() * CapsuleRadius() * 2;
		FVector End = bWallRunIsRight ? Start + TraceLenght : Start - TraceLenght;
		
		FCollisionQueryParams Params = FCollisionQueryParams(FName(TEXT("WallRun")), true, CharacterOwner);
		TArray<AActor*> ChildActors;
		CharacterOwner->GetAllChildActors(ChildActors);
		Params.AddIgnoredActors(ChildActors);
		
		FHitResult WallHit;
		GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, WallDetectionChannel, Params);

		// Check if acceleration direction is exceeding the pull away angle
		float SinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallRunPullAwayAngle));
		bool bWantsToPullAway = WallHit.IsValidBlockingHit() && !Acceleration.IsNearlyZero() && (Acceleration.GetSafeNormal() | WallHit.Normal) > SinPullAwayAngle;
		if (!WallHit.IsValidBlockingHit() || bWantsToPullAway)
		{
			StopWallRun();
			return;
		}
		
		// Clamp Acceleration
		Acceleration = FVector::VectorPlaneProject(Acceleration, WallHit.Normal);
		Acceleration.Z = 0.f;
		// Apply acceleration
		CalcVelocity(timeTick, 0.f, false, GetMaxBrakingDeceleration());
		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
		
		//Apply gravity based on the acceleration direction
		bool bVelUp = Velocity.Z > 0.f;
		if (IsValid(AccelerationToGravityScaleCurve))
		{
			float TangentAccel = Acceleration.GetSafeNormal() | Velocity.GetSafeNormal2D();
			Velocity.Z += GetGravityZ() * AccelerationToGravityScaleCurve->GetFloatValue(bVelUp ? 0.f : TangentAccel) * timeTick;
		}
		else if (bVelUp)
		{
			Velocity.Z += GetGravityZ() * timeTick;
			Velocity.Z *= FMath::IsNearlyZero(Velocity.Z) ? 0.f : 1.f;
		}

		// Apply gravity based on the time
		WallRunningElapsedTime += timeTick;
		if (IsValid(TimeToGravityScaleCurve))
		{
			Velocity.Z += GetGravityZ() * TimeToGravityScaleCurve->GetFloatValue(WallRunningElapsedTime) * timeTick;
		}

		// Check if we are reached the max wall run duration
		if (WallRunningElapsedTime >= MaxWallRunDuration && MaxWallRunDuration > 0.f)
		{
			bWallRunMaxDurationReached = true;
			StopWallRun();
			return;
		}

		// Check is speed is too low
		if (Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2) || Velocity.Z < -MaxVerticalWallRunSpeed)
		{
			StopWallRun();
			return;
		}
		
		// Apply the movement to the character
		const FVector Delta = timeTick * Velocity; 
		const bool bZeroDelta = Delta.IsNearlyZero();
		if ( bZeroDelta )
		{
			RemainingTime = 0.f;
		}
		else
		{
			FHitResult Hit;
			FQuat WallRunRotation = FVector::UpVector.Cross(WallHit.Normal * (bWallRunIsRight ? 1.f : -1.f)).Rotation().Quaternion();
			SafeMoveUpdatedComponent(Delta, WallRunRotation, true, Hit);
			FVector WallAttractionDelta = -WallHit.Normal * WallAttractionForce * timeTick;
			SafeMoveUpdatedComponent(WallAttractionDelta, WallRunRotation, true, Hit);
		}
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			RemainingTime = 0.f;
			break;
		}
	}

	
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector TraceLenght = UpdatedComponent->GetRightVector() * CapsuleRadius() * 2.f;
	FVector End = bWallRunIsRight ? Start + TraceLenght : Start - TraceLenght;
	
	FCollisionQueryParams Params = FCollisionQueryParams(FName(TEXT("WallRun")), true, CharacterOwner);
	TArray<AActor*> ChildActors;
	CharacterOwner->GetAllChildActors(ChildActors);
	Params.AddIgnoredActors(ChildActors);
	
	FHitResult FloorHit, WallHit;
	GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, WallDetectionChannel, Params);
	GetWorld()->LineTraceSingleByProfile(FloorHit, Start, Start + FVector::DownVector * (CapsuleHalfHeight() + MinWallRunHeight * .5f), "BlockAll", Params);
	if (FloorHit.IsValidBlockingHit() || !WallHit.IsValidBlockingHit() || Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2))
	{
		SetMovementMode(MOVE_Falling);
		OnWallRunEnded.Broadcast();
	}
}

bool UGvCharacterMovementComponent::TryRailGrind(USplineComponent* Spline)
{
	if (!bCanRailGrind) return false;
	if (!IsValid(Spline)) return false;
	//if (!IsFalling()) return false;
	RailSpline = Spline;
	SetMovementMode(MOVE_Custom, CMOVE_RailGrind);
	float RailDist = RailSpline->GetDistanceAlongSplineAtLocation(UpdatedComponent->GetComponentLocation(), ESplineCoordinateSpace::World);
	RailGrindStartLocation = RailSpline->GetLocationAtDistanceAlongSpline(RailDist, ESplineCoordinateSpace::World);
	bRailGrindWasStarted = RailGrindStartLocation.Equals(CapsuleBaseLocation() - CharacterOwner->GetActorUpVector() * DistanceOffset);
	CalcGrindDirection(RailDist);
	OnRailGrindStarted.Broadcast();
	return true;
}

void UGvCharacterMovementComponent::PhysRailGrinding(float deltaTime, int32 Iterations)
{
	// All line marked by empty comments "/**/" are to follow the same logic as PhysWalking

	/**/
	if (deltaTime < MIN_TICK_TIME) 
	{
		return;
	}
	/**/
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}
	
	TFunction<FVector()> CharacterLocationOffset = [&]() -> FVector {
		return CapsuleBaseLocation() - CharacterOwner->GetActorUpVector() * DistanceOffset;
	};

	bJustTeleported = false; /**/
	float RemainingTime = deltaTime; /**/
	/**/
	while ( (RemainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)) )
	{
		Iterations++; /**/
		bJustTeleported = false; /**/
		const float timeTick = GetSimulationTimeStep(RemainingTime, Iterations); /**/
		RemainingTime -= timeTick; /**/

		// before grinding moves the character to the rail start location
		FVector CharacterLocation = CharacterLocationOffset();
		if (!RailGrindStartLocation.Equals(CharacterLocation, RailAttractionForce * timeTick) && !bRailGrindWasStarted)
		{
			FHitResult Hit;
			const FVector RailAttractionDelta = (RailGrindStartLocation - CharacterLocation).GetSafeNormal() * RailAttractionForce * timeTick;
			SafeMoveUpdatedComponent(RailAttractionDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
			bRailGrindWasStarted = RailGrindStartLocation.Equals(CharacterLocationOffset(), RailAttractionForce * timeTick);
		}
		else
		{
			// if the character location is equal to the rail start location
			// from the first frame set this bool to true to avoid being stuck
			bRailGrindWasStarted = true;
		}
		
		if (!bRailGrindWasStarted) continue;

		// Check if we are reached the end of the rail
		float RailDist = RailSpline->GetDistanceAlongSplineAtLocation(CharacterLocationOffset(), ESplineCoordinateSpace::World);
		if (RailDist + GrindingDirection <= 0.f || RailDist + GrindingDirection >= RailSpline->GetSplineLength())
		{
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(RemainingTime, Iterations);
			RailSpline = nullptr;
			OnRailGrindEnded.Broadcast();
			return;
		}

		// Calculate the direction of the grind
		FVector CurrentLocation = RailSpline->GetLocationAtDistanceAlongSpline(RailDist, ESplineCoordinateSpace::World);
		FVector NextLocation = RailSpline->GetLocationAtDistanceAlongSpline(RailDist + GrindingDirection, ESplineCoordinateSpace::World);
		Acceleration = (NextLocation - CurrentLocation).GetSafeNormal() * MaxAcceleration;
		Velocity = (NextLocation - CurrentLocation).GetSafeNormal() * MaxRailGrindSpeed;

		// Calculate the new rotation
		FRotator NewRotation = RailSpline->GetTransformAtDistanceAlongSpline(RailDist, ESplineCoordinateSpace::World).Rotator();
		NewRotation.Yaw += GrindingDirection == 1.f ? 0.f : 180.f;
		NewRotation.Pitch *= GrindingDirection;
		NewRotation.Roll *= GrindingDirection;
		FQuat NewRotationQuat = NewRotation.Quaternion();

		// Apply the velocity to the character
		FHitResult Hit;
		const FVector Delta = timeTick * Velocity;
		SafeMoveUpdatedComponent(Delta, NewRotationQuat, true, Hit);

		// Continuously adjust the character to the rail
		CharacterLocation = CharacterLocationOffset();
		RailDist = RailSpline->GetDistanceAlongSplineAtLocation(CharacterLocation, ESplineCoordinateSpace::World);
		FVector AdjustLocation = RailSpline->GetLocationAtDistanceAlongSpline(RailDist + GrindingDirection, ESplineCoordinateSpace::World);
		const FVector RailAttractionDelta = (AdjustLocation - CharacterLocation);
		// Apply adjustment only if the angle between adjustment delta and velocity is less than 90 degrees
		if ((RailAttractionDelta.GetSafeNormal() | Velocity.GetSafeNormal()) >= 0.f )
		{
			SafeMoveUpdatedComponent(RailAttractionDelta, NewRotationQuat, true, Hit);
		}
	}
}

void UGvCharacterMovementComponent::CalcGrindDirection(const float RailDist)
{
	const FVector LocationA = RailSpline->GetLocationAtDistanceAlongSpline(RailDist + 1.f, ESplineCoordinateSpace::World);
	const FVector LocationB = RailSpline->GetLocationAtDistanceAlongSpline(RailDist - 1.f, ESplineCoordinateSpace::World);
	const FVector DirectionA = (LocationA - RailGrindStartLocation).GetSafeNormal();
	const FVector DirectionB = (LocationB - RailGrindStartLocation).GetSafeNormal();
	const float DotA = CharacterOwner->GetActorForwardVector() | DirectionA;
	const float DotB = CharacterOwner->GetActorForwardVector() | DirectionB;
	if (DotA >= DotB)
	{
		GrindingDirection = 1.f;
	}
	else
	{
		GrindingDirection = -1.f;
	}
}

bool UGvCharacterMovementComponent::CanCoyoteJump() const
{
	return bCoyoteJumpEnabled && CoyoteTimeElapsed <= CoyoteTime && Velocity.Z < 0.f;
}

