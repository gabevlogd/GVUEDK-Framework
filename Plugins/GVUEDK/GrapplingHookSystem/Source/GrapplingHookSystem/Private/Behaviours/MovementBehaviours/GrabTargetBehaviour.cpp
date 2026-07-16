// Copyright Villains, Inc. All Rights Reserved.


#include "Behaviours/MovementBehaviours/GrabTargetBehaviour.h"

#include <string>

#include "Curves/CurveVector.h"
#include "Components/GrapplingHookComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UGrabTargetBehaviour::UGrabTargetBehaviour()
{
	bIsHooking = false;
	bMotionDataCalculated = false;
	bApplyMomentumDuringHookThrow = false;
	bMaxExtensionReached = false;
	bShowDebug = false;
	bOrientGrabbedActorRotationToMovement = true;
	bCanGrabTargetOnHookReduction = false;
	MaxHookExtension = 2000.f;
	HookLinearSpeed = 2000.f;
	StartDelay = 0.f;
	StartDelayTimer = 0.f;
	HookAttachedTimer = 0.f;
	HookSpeedCurve = nullptr;
	ExtensionDirection = FVector::ZeroVector;
	TargetGrabbedLocation = FVector::ZeroVector;
	AimTraceChannel = ECC_Visibility;
	StopHookTraceChannel = ECC_Visibility;
}

bool UGrabTargetBehaviour::StartHooking()
{
	Super::StartHooking();
	if (bIsHooking)
	{
		return false;
	}
	
	bIsHooking = true;
	StartDelayTimer = 0.f;
	HookAttachedTimer = 0.f;
	bMaxExtensionReached = false;
	bMotionDataCalculated = false;

	StopGrabOnLastValidTarget();
	
	GrapplingHookComponent->OnStartHooking.Broadcast();
	return true;
}

void UGrabTargetBehaviour::StopHooking()
{
	Super::StopHooking();
	if (IsTargetAcquired())
	{
		LastValidTarget = GetTargetGrabPoint();
		GetWorld()->GetTimerManager().SetTimer(StopGrabOnLastValidTargetTimerHandle, this, &UGrabTargetBehaviour::StopGrabOnLastValidTarget, TargetStopGrabDelay, false);
	}
	GrapplingHookComponent->SetOwnerMovingTowardTarget(false);
	GrapplingHookComponent->SetHookHitObstacle(false);
	GrapplingHookComponent->SetTargetGrabPoint(nullptr);
	bIsHooking = false;
	TargetGrabbedLocation = FVector::ZeroVector;
	ExtensionDirection = FVector::ZeroVector;
	if (!bApplyMomentumDuringHookThrow)
	{
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(PreviousMovementMode);
	}
	
	GrapplingHookComponent->OnStopHooking.Broadcast();
}

bool UGrabTargetBehaviour::TickMode(float DeltaTime)
{
	Super::TickMode(DeltaTime);
	if (bIsHooking)
	{
		
			
		if (StartDelayTimer < StartDelay)
		{
			//orient character rotation to the camera view pointing direction (Control Rotation)
			FRotator NewRotation = OwnerCharacter->GetControlRotation();
			NewRotation.Pitch = 0.f;
			OwnerCharacter->SetActorRotation(NewRotation);
			
			StartDelayTimer += DeltaTime;
			return true; // do not want serch mode to tick before the start of hook motion
		}

		if (!bMotionDataCalculated) // calculate motion data only once
		{
			if (TryCalculateMotionData())
			{
				bMotionDataCalculated = true;
				if (!bApplyMomentumDuringHookThrow)
				{
					PreviousMovementMode = OwnerCharacter->GetCharacterMovement()->MovementMode;
					OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
					OwnerCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;
				}
				GrapplingHookComponent->OnHookMotionStarted.Broadcast();
			}
			else
			{
				GrapplingHookComponent->OnInterruptHooking.Broadcast();
				StopHooking();
				return true;
			}
		}
		
		if (IsTargetAcquired())
		{
			// snap the hook to the target during the first frame after the target is acquired
			if (!bMaxExtensionReached) 
			{
				GrapplingHookComponent->SetHookLocation(GetTargetGrabPoint()->Execute_GetLocation(GetTargetGrabPoint()->_getUObject()));
				TargetGrabbedLocation = GrapplingHookComponent->GetHookLocation();
			}
			
			bMaxExtensionReached = true;
			// GetTargetGrabPoint()->Execute_SetLocation(GetTargetGrabPoint()->_getUObject(), GrapplingHookComponent->GetHookLocation());
			// if (bOrientGrabbedActorRotationToMovement)
			// 	OrientGrabbedActorRotationToMovement();

			if (HookAttachedTimer < HookAttachedDelay)
			{
				HookAttachedTimer += DeltaTime;
				return true;
			}
		}
		else if (GrapplingHookComponent->HasHookHitObstacle())
        {
			bMaxExtensionReached = true;
        }
		
#if !UE_BUILD_SHIPPING
		if (bShowDebug)
		{
			DrawDebugBox(OwnerCharacter->GetWorld(), GrapplingHookComponent->GetHookLocation(), FVector(30.f), FColor::Blue, false, 0.01f);
			DrawDebugLine(OwnerCharacter->GetWorld(), GetHookStartLocationSocket(), GrapplingHookComponent->GetHookLocation(), FColor::Purple, false, 0.01f);
		}
#endif
		
		return PerformMotion(DeltaTime);
	}
	return true; 
}

bool UGrabTargetBehaviour::TryCalculateMotionData()
{
	ExtensionDirection = GetAimDirection();
	GrapplingHookComponent->SetHookLocation(GetHookStartLocationSocket());
	HookStartSocketRelativeLocation = GetHookStartLocationSocket() - OwnerCharacter->GetActorLocation();
	return true;
}

float UGrabTargetBehaviour::GetHookSpeed() const
{
	if (!IsValid(HookSpeedCurve))
	{
		return HookLinearSpeed;
	}

	if (bMaxExtensionReached)
	{
		if (IsTargetAcquired())
			// speed for the way back motion with target
			return HookSpeedCurve->GetVectorValue(GetCurrentHookExtensionNormalized()).Y * 100.f; // convert from m/s to cm/s

		// speed for the way back motion without target
		return HookSpeedCurve->GetVectorValue(GetCurrentHookExtensionNormalized()).Z * 100.f; // convert from m/s to cm/s
	}

	// speed for the way out motion
	return HookSpeedCurve->GetVectorValue(GetCurrentHookExtensionNormalized()).X * 100.f; // convert from m/s to cm/s
}

float UGrabTargetBehaviour::GetCurrentHookExtensionNormalized() const
{
	float MaxExtensionSquared = MaxHookExtension * MaxHookExtension;
	if (TargetGrabbedLocation != FVector::ZeroVector)
	{
		MaxExtensionSquared = FVector::DistSquared(GetHookEndLocationSocket(), TargetGrabbedLocation);
	}
	return FVector::DistSquared(GrapplingHookComponent->GetHookLocation(), bMaxExtensionReached ? GetHookEndLocationSocket() : GetHookStartLocationSocket()) / MaxExtensionSquared;
}

FTransform UGrabTargetBehaviour::GetOwnerTransform() const
{
	FTransform Transform = OwnerCharacter->GetTransform();
	Transform.SetRotation(FQuat::Identity);
	return Transform;
}

FVector UGrabTargetBehaviour::GetHookStartLocationSocket() const
{
	return OwnerCharacter->GetMesh()->GetSocketLocation(HookStartLocationSocket);
}

FVector UGrabTargetBehaviour::GetHookEndLocationSocket() const
{
    return OwnerCharacter->GetMesh()->GetSocketLocation(HookEndLocationSocket);
}

FVector UGrabTargetBehaviour::GetAimDirection() const
{
	FVector AimDirection;
	const FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(OwnerCharacter ,0)->GetCameraCacheView().Location;
	const FVector StartTraceLocationWithOffset = CameraLocation + OwnerCharacter->GetControlRotation().Vector() * 200.f;
	const FVector PointedLocation = CameraLocation + OwnerCharacter->GetControlRotation().Vector() * MaxHookExtension;
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTraceLocationWithOffset, PointedLocation, AimTraceChannel, CollisionParams))
	{
		AimDirection = (HitResult.ImpactPoint - GetHookStartLocationSocket()).GetSafeNormal();
	}
	else AimDirection = (PointedLocation - GetHookStartLocationSocket()).GetSafeNormal();

#if !UE_BUILD_SHIPPING
	if (bShowDebug)
	{
		if (HitResult.bBlockingHit)
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 20.f, FColor::Red, false, 5.f);
		DrawDebugLine(GetWorld(), StartTraceLocationWithOffset, PointedLocation, FColor::Green, false, 5.f);
	}
#endif

	return AimDirection;
}

bool UGrabTargetBehaviour::IsMaxExtensionReached() const
{
	return bMaxExtensionReached || GetCurrentHookExtensionNormalized() >= 1.f;
}

bool UGrabTargetBehaviour::PerformMotion(float DeltaTime)
{
	
	auto UpdateOwnerRotation = [this](float DeltaTime)
	{
		const FVector HookStartLocation = OwnerCharacter->GetActorLocation() + HookStartSocketRelativeLocation;
		const FVector Delta = GrapplingHookComponent->GetHookLocation() - HookStartLocation;

		// Ignore the vertical component of the hook direction to avoid unwanted pitch rotations when the hook is above or below the player.
		const FVector HorizontalDirection(
			Delta.X,
			Delta.Y,
			0.f);

		// if the hook is nearly perfectly vertical respect to the player, do not update the player rotation to avoid unwanted rotations due to small variations of the hook position
		constexpr float MinHorizontalDistance = 5.f;

		if (HorizontalDirection.SizeSquared() < FMath::Square(MinHorizontalDistance))
		{
			return;
		}

		const FRotator TargetRotation =
			HorizontalDirection.Rotation();

		const FQuat NewQuat = FQuat::Slerp(
			OwnerCharacter->GetActorQuat(),
			TargetRotation.Quaternion(),
			DeltaTime * 10.f);

		OwnerCharacter->SetActorRotation(NewQuat);
	};
	
	GrapplingHookComponent->OnPerformHookMotion.Broadcast();
	
	FVector NewHookLocation;
	if (IsMaxExtensionReached()) 
	{
		bMaxExtensionReached = true;
		
		const FVector ReductionDirection = (GetHookEndLocationSocket() - GrapplingHookComponent->GetHookLocation()).GetSafeNormal();

		// roba schifosa buttata qui giusto per funzionare, tanto in ogni caso devo rifattorizare tutto sto sistema di merda
		if (IsTargetAcquired())
		{
			IGrabPoint* Target = GrapplingHookComponent->GetTargetGrabPoint();
			if (Target->Execute_GetMass(Target->_getUObject()) < 1) // Assume owner mass is always 1
			{
				FCollisionQueryParams CollisionParams;
				CollisionParams.AddIgnoredActor(OwnerCharacter);
				FHitResult HitResult;
				bool SweepResult = GetWorld()->SweepSingleByChannel(HitResult, GetHookEndLocationSocket(), GetHookEndLocationSocket(),
					FQuat::Identity, StopHookTraceChannel, FCollisionShape::MakeSphere(ObstaclesDetectionRadius), CollisionParams);
				//DrawDebugSphere(GetWorld(), GetHookEndLocationSocket(), ObstaclesDetectionRadius, 12, SweepResult ? FColor::Red : FColor::Green, false, -1.f);
				
				if (SweepResult || FVector::Distance(GetHookEndLocationSocket(), GetHookLocation()) <= StopGrabDistance)
				{
					StopHooking();
					return true; // do not want search mode to tick after the end of hook motion
				}
				
				NewHookLocation = GrapplingHookComponent->GetHookLocation() + ReductionDirection * GetHookSpeed() * DeltaTime;
				GrapplingHookComponent->SetHookLocation(NewHookLocation);
				
				Target->Execute_SetLocation(Target->_getUObject(), GrapplingHookComponent->GetHookLocation());
				if (bOrientGrabbedActorRotationToMovement)
					OrientGrabbedActorRotationToMovement();
				
				Target->Execute_OnMoving(Target->_getUObject());
				
				UpdateOwnerRotation(DeltaTime);
				return true;
			}
			else
			{
				GrapplingHookComponent->SetOwnerMovingTowardTarget(true);
				const FVector NextLocation = OwnerCharacter->GetActorLocation() - ReductionDirection * GetHookSpeed() * DeltaTime;
				OwnerMovementDirection = (NextLocation - OwnerCharacter->GetActorLocation()).GetSafeNormal();
				OwnerCharacter->SetActorLocation(NextLocation);

				//avoid to go through objects while going to the target
				FCollisionQueryParams CollisionParams;
				CollisionParams.AddIgnoredActor(OwnerCharacter);
				FHitResult HitResult;
				bool SweepResult = GetWorld()->SweepSingleByChannel(HitResult, GetHookEndLocationSocket(), GetHookEndLocationSocket(),
					FQuat::Identity, StopHookTraceChannel, FCollisionShape::MakeSphere(ObstaclesDetectionRadius), CollisionParams);
				if (SweepResult || FVector::Distance(GetHookEndLocationSocket(), GetHookLocation()) <= StopGrabDistance)
				{
					StopHooking();
					return true;
				}
#if !UE_BUILD_SHIPPING
				if (bShowDebug)
				{
					DrawDebugSphere(GetWorld(), GetHookEndLocationSocket(), ObstaclesDetectionRadius, 12, FColor::Green, false, -1.f);
				}
#endif
				
				UpdateOwnerRotation(DeltaTime);
				return true; 
			}
		}
		//fine roba schifosa 

		NewHookLocation = GrapplingHookComponent->GetHookLocation() + ReductionDirection * GetHookSpeed() * DeltaTime;
		GrapplingHookComponent->SetHookLocation(NewHookLocation);
		
		UpdateOwnerRotation(DeltaTime);
		
		FCollisionQueryParams CollisionParams;
		FHitResult HitResult;
		bool SweepResult = GetWorld()->SweepSingleByChannel(HitResult, NewHookLocation, NewHookLocation,
			FQuat::Identity, StopHookTraceChannel, FCollisionShape::MakeSphere(ObstaclesDetectionRadius), CollisionParams);
		//DrawDebugSphere(GetWorld(), NewHookLocation, ObstaclesDetectionRadius, 12, SweepResult ? FColor::Red : FColor::Green, false, -1.f);
		if (SweepResult || FVector::Distance(GetHookEndLocationSocket(), GetHookLocation()) <= StopGrabDistance)
		{
			StopHooking();
			return true; // do not want search mode to tick after the end of hook motion
		}

		if (bCanGrabTargetOnHookReduction)
		{
			return IsTargetAcquired();
		}
		return true;
	}
	
	NewHookLocation = GrapplingHookComponent->GetHookLocation() + ExtensionDirection * GetHookSpeed() * DeltaTime;
	GrapplingHookComponent->SetHookLocation(NewHookLocation);
	
	UpdateOwnerRotation(DeltaTime);
	
	return false; // continue search mode tick
}

void UGrabTargetBehaviour::OrientGrabbedActorRotationToMovement() const 
{
	const FVector MoveDirection = (OwnerCharacter->GetActorLocation() - GetTargetGrabPoint()->Execute_GetLocation(GetTargetGrabPoint()->_getUObject())).GetSafeNormal();
	FRotator MovementRotation = MoveDirection.Rotation();
	MovementRotation.Pitch = 0.f;
	GetTargetGrabPoint()->Execute_GetActor(GetTargetGrabPoint()->_getUObject())->SetActorRotation(MovementRotation);
}

void UGrabTargetBehaviour::StopGrabOnLastValidTarget()
{
	if (LastValidTarget != nullptr)
	{
		LastValidTarget->Execute_OnStopGrab(LastValidTarget->_getUObject());
		LastValidTarget = nullptr;
		StopGrabOnLastValidTargetTimerHandle.Invalidate();
	}
}
