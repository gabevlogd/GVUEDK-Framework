// Copyright Villains, Inc. All Rights Reserved.


#include "Behaviours/MovementBehaviours/GHSelfTowardTarget.h"

#include "Components/GrapplingHookComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGHSelfTowardTarget::UGHSelfTowardTarget()
{
	bIsHooking = false;
	bMotionDataCalculated = false;
	bOrientRotationToMovement = true;
	bApplyMomentumDuringHookThrow = false;
	LinearSpeed = 2500.f;
	TotalRelativeSquaredDistance = 0.f;
	StartDelay = 0.f;
	StartDelayTimer = 0.f;
	StartRelativeLocation = FVector::ZeroVector;
	SpeedCurve = nullptr;
}

bool UGHSelfTowardTarget::StartHooking()
{
	Super::StartHooking();
	if (!IsTargetAcquired() || bIsHooking)
	{
		return false;
	}
	StartDelayTimer = 0.f;
	bIsHooking = true;
	PreviousMovementMode = OwnerCharacter->GetCharacterMovement()->MovementMode;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	if (!bApplyMomentumDuringHookThrow)
	{
		OwnerCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
	GetTargetGrabPoint()->Execute_Unhighlight(GetTargetGrabPoint()->_getUObject());
	GrapplingHookComponent->OnStartHooking.Broadcast();
	return true;
}

void UGHSelfTowardTarget::StopHooking()
{
	Super::StopHooking();
	GrapplingHookComponent->SetTargetGrabPoint(nullptr);
	bMotionDataCalculated = false;
	bIsHooking = false;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(PreviousMovementMode);
	GrapplingHookComponent->SetOwnerMovingTowardTarget(false);
	GrapplingHookComponent->OnStopHooking.Broadcast();
}

bool UGHSelfTowardTarget::TickMode(float DeltaTime)
{
	Super::TickMode(DeltaTime);
	if (bIsHooking)
	{
		if (bOrientRotationToMovement)
		{
			OrientRotationToMovement(DeltaTime);
		}
		
		if (StartDelayTimer < StartDelay)
		{
			StartDelayTimer += DeltaTime;
			return true;
		}
		
		if (!bMotionDataCalculated)
		{
			if (TryCalculateMotionData())
			{
				bMotionDataCalculated = true;
				GrapplingHookComponent->OnHookMotionStarted.Broadcast();
			}
			else
			{
				GrapplingHookComponent->OnInterruptHooking.Broadcast();
				StopHooking();
				return false;
			}
		}
		
		PerformMotion(DeltaTime);
		return true;
	}
	return false;
}

void UGHSelfTowardTarget::PerformMotion(float DeltaTime)
{
	//TODO:
	// if (CheckObstacles())
	// {
	// 	StopHooking();
	// 	return;
	// }

	GrapplingHookComponent->SetOwnerMovingTowardTarget(true);
	OwnerCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	OwnerCharacter->SetActorLocation(OwnerCharacter->GetActorLocation() + GetDirection() * GetSpeed() * DeltaTime);
	GrapplingHookComponent->OnPerformHookMotion.Broadcast();
	
	if (OwnerCharacter->GetActorLocation().Equals(GetEndLocation(), GetSpeed() * DeltaTime))
	{
		OwnerCharacter->SetActorLocation(GetEndLocation());
		StopHooking();
	}
}

void UGHSelfTowardTarget::OrientRotationToMovement(float DeltaTime)
{
	const FVector LandingPoint = GetTargetGrabPoint()->Execute_GetLandingPoint(GetTargetGrabPoint()->_getUObject());
	FRotator TargetRotation = (LandingPoint - OwnerCharacter->GetActorLocation()).Rotation();
	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;
	const FRotator NewRotation = FMath::RInterpConstantTo(OwnerCharacter->GetActorRotation(), TargetRotation, DeltaTime, 1000.f);
	OwnerCharacter->SetActorRotation(NewRotation);
}

bool UGHSelfTowardTarget::TryCalculateMotionData()
{
	if (CheckObstacles()) 
	{
		return false;
	}

	// Cache the start location relative to the target grab point
	const AActor* TargetGrabPointActor = GetTargetGrabPoint()->Execute_GetActor(GetTargetGrabPoint()->_getUObject());
	StartRelativeLocation = TargetGrabPointActor->GetActorTransform().InverseTransformPosition(OwnerCharacter->GetActorLocation());

	// Also cache the total relative distance between the start and end locations
	TotalRelativeSquaredDistance = FVector::DistSquared(OwnerCharacter->GetActorLocation(), GetEndLocation());
	
	return true;
}

float UGHSelfTowardTarget::GetSpeed()
{
	if (!IsValid(SpeedCurve))
	{
		return LinearSpeed;
	}
	const FVector CurrentRelativeLocation = GetTargetGrabPoint()->Execute_GetActor(GetTargetGrabPoint()->_getUObject())->GetActorTransform().InverseTransformPosition(OwnerCharacter->GetActorLocation());
	const float CurveInput = FVector::DistSquared(CurrentRelativeLocation, StartRelativeLocation) / TotalRelativeSquaredDistance;
	return SpeedCurve->GetFloatValue(CurveInput);
}

FVector UGHSelfTowardTarget::GetEndLocation() const
{
	if (!IsTargetAcquired())
	{
		return FVector::ZeroVector;
	}
	return GetTargetGrabPoint()->Execute_GetLandingPoint(GetTargetGrabPoint()->_getUObject());
}

FVector UGHSelfTowardTarget::GetDirection() const
{
	return (GetEndLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal();
}

bool UGHSelfTowardTarget::CheckObstacles() const 
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	AActor* TargetGrabPointActor = Cast<AActor>(GetTargetGrabPoint());
	CollisionParams.AddIgnoredActor(TargetGrabPointActor);
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacter->GetActorLocation(), GetTargetGrabPoint()->Execute_GetLocation(TargetGrabPointActor), ObstaclesTraceChannel, CollisionParams))
	{
		return true;
	}
	return false;
}
