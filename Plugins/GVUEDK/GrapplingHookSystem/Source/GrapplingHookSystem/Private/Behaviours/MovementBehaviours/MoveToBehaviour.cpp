// Copyright Villains, Inc. All Rights Reserved.


#include "Behaviours/MovementBehaviours/MoveToBehaviour.h"

#include "Components/GrapplingHookComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UMoveToBehaviour::UMoveToBehaviour()
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

bool UMoveToBehaviour::StartHooking()
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

void UMoveToBehaviour::StopHooking()
{
	Super::StopHooking();
	GrapplingHookComponent->SetTargetGrabPoint(nullptr);
	GrapplingHookComponent->bNearGrapplePointBroadcasted = false;
	bMotionDataCalculated = false;
	bIsHooking = false;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(PreviousMovementMode);
	GrapplingHookComponent->SetOwnerMovingTowardTarget(false);
	GrapplingHookComponent->OnStopHooking.Broadcast();
}

bool UMoveToBehaviour::TickMode(float DeltaTime)
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

void UMoveToBehaviour::PerformMotion(float DeltaTime)
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

	
	if (!GrapplingHookComponent->bNearGrapplePointBroadcasted &&
		FVector::DistSquared(OwnerCharacter->GetActorLocation(), GetEndLocation()) <= FMath::Square(NearGrapplePointDistance))
	{
		GrapplingHookComponent->OnNearGrapplePoint.Broadcast();
		GrapplingHookComponent->bNearGrapplePointBroadcasted = true;
	}
	
	if (OwnerCharacter->GetActorLocation().Equals(GetEndLocation(), GetSpeed() * DeltaTime))
	{
		OwnerCharacter->SetActorLocation(GetEndLocation());
		StopHooking();
	}
}

void UMoveToBehaviour::OrientRotationToMovement(float DeltaTime)
{
	const FVector LandingPoint = GetTargetGrabPoint()->Execute_GetLandingPoint(GetTargetGrabPoint()->_getUObject());
	FRotator TargetRotation = (LandingPoint - OwnerCharacter->GetActorLocation()).Rotation();
	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;
	const FRotator NewRotation = FMath::RInterpConstantTo(OwnerCharacter->GetActorRotation(), TargetRotation, DeltaTime, 1000.f);
	OwnerCharacter->SetActorRotation(NewRotation);
}

bool UMoveToBehaviour::TryCalculateMotionData()
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

float UMoveToBehaviour::GetSpeed()
{
	if (!IsValid(SpeedCurve))
	{
		return LinearSpeed;
	}
	const FVector CurrentRelativeLocation = GetTargetGrabPoint()->Execute_GetActor(GetTargetGrabPoint()->_getUObject())->GetActorTransform().InverseTransformPosition(OwnerCharacter->GetActorLocation());
	const float CurveInput = FVector::DistSquared(CurrentRelativeLocation, StartRelativeLocation) / TotalRelativeSquaredDistance;
	return SpeedCurve->GetFloatValue(CurveInput);
}

FVector UMoveToBehaviour::GetEndLocation() const
{
	if (!IsTargetAcquired())
	{
		return FVector::ZeroVector;
	}
	return GetTargetGrabPoint()->Execute_GetLandingPoint(GetTargetGrabPoint()->_getUObject());
}

FVector UMoveToBehaviour::GetDirection() const
{
	return (GetEndLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal();
}

bool UMoveToBehaviour::CheckObstacles() const 
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
