// Copyright Villains, Inc. All Rights Reserved.


#include "Behaviours/MovementBehaviours/GHTargetTowardSelf.h"

#include <string>

#include "Curves/CurveVector.h"
#include "Components/GrapplingHookComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UGHTargetTowardSelf::UGHTargetTowardSelf()
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
}

bool UGHTargetTowardSelf::StartHooking()
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

void UGHTargetTowardSelf::StopHooking()
{
	Super::StopHooking();
	if (IsTargetAcquired())
	{
		LastValidTarget = GetTargetGrabPoint();
		GetWorld()->GetTimerManager().SetTimer(StopGrabOnLastValidTargetTimerHandle, this, &UGHTargetTowardSelf::StopGrabOnLastValidTarget, TargetStopGrabDelay, false);
	}
	GrapplingHookComponent->SetOwnerMovingTowardTarget(false);
	GrapplingHookComponent->SetHookHitObstacle(false);
	GrapplingHookComponent->SetTargetGrabPoint(nullptr);
	bIsHooking = false;
	TargetGrabbedLocation = FVector::ZeroVector;
	if (!bApplyMomentumDuringHookThrow)
	{
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(PreviousMovementMode);
	}
	
	GrapplingHookComponent->OnStopHooking.Broadcast();
}

bool UGHTargetTowardSelf::TickMode(float DeltaTime)
{
	Super::TickMode(DeltaTime);
	if (bIsHooking)
	{
		
		if (StartDelayTimer < StartDelay)
		{
			//orient character rotation to the direction of the hook motion
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
			GetTargetGrabPoint()->Execute_SetLocation(GetTargetGrabPoint()->_getUObject(), GrapplingHookComponent->GetHookLocation());
			if (bOrientGrabbedActorRotationToMovement)
				OrientGrabbedActorRotationToMovement();

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

bool UGHTargetTowardSelf::TryCalculateMotionData()
{
	ExtensionDirection = GetAimDirection();
	GrapplingHookComponent->SetHookLocation(GetHookStartLocationSocket());
	return true;
}

float UGHTargetTowardSelf::GetHookSpeed() const
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

float UGHTargetTowardSelf::GetCurrentHookExtensionNormalized() const
{
	float MaxExtensionSquared = MaxHookExtension * MaxHookExtension;
	if (TargetGrabbedLocation != FVector::ZeroVector)
	{
		MaxExtensionSquared = FVector::DistSquared(GetHookEndLocationSocket(), TargetGrabbedLocation);
	}
	return FVector::DistSquared(GrapplingHookComponent->GetHookLocation(), bMaxExtensionReached ? GetHookEndLocationSocket() : GetHookStartLocationSocket()) / MaxExtensionSquared;
}

FTransform UGHTargetTowardSelf::GetOwnerTransform() const
{
	FTransform Transform = OwnerCharacter->GetTransform();
	Transform.SetRotation(FQuat::Identity);
	return Transform;
}

FVector UGHTargetTowardSelf::GetHookStartLocationSocket() const
{
	return OwnerCharacter->GetMesh()->GetSocketLocation(HookStartLocationSocket);
}

FVector UGHTargetTowardSelf::GetHookEndLocationSocket() const
{
    return OwnerCharacter->GetMesh()->GetSocketLocation(HookEndLocationSocket);
}

FVector UGHTargetTowardSelf::GetAimDirection() const
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

bool UGHTargetTowardSelf::IsMaxExtensionReached() const
{
	return bMaxExtensionReached || GetCurrentHookExtensionNormalized() >= 1.f;
}

bool UGHTargetTowardSelf::IsMinExtensionReached(const float DeltaTime) const
{
	const float SquaredTolerance = (GetHookSpeed() * DeltaTime) * (GetHookSpeed() * DeltaTime);
	return FMath::IsNearlyZero(FVector::DistSquared(GrapplingHookComponent->GetHookLocation(), GetHookEndLocationSocket()), SquaredTolerance);
}

bool UGHTargetTowardSelf::PerformMotion(float DeltaTime)
{
	//if (FVector::DistSquared(GrapplingHookComponent->GetHookLocation(), GetHookStartLocationSocket()) > MaxHookExtension * MaxHookExtension)
    //{
    //    StopHooking();
	//	return true;
    //}
	
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
				Target->Execute_OnMoveing(Target->_getUObject());
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
				//CollisionParams.AddIgnoredActor(Target->Execute_GetActor(Target->_getUObject()));
				FHitResult HitResult;
				if (GetWorld()->SweepSingleByChannel(HitResult, GetHookStartLocationSocket(), GetHookStartLocationSocket(),
					FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(ObstaclesDetectionRadius), CollisionParams) || IsMinExtensionReached(DeltaTime))
				{
					StopHooking();
					return true;
				}
#if !UE_BUILD_SHIPPING
				if (bShowDebug)
				{
					DrawDebugSphere(GetWorld(), GetHookStartLocationSocket(), 20.f, 12, FColor::Green, false, -1.f);
				}
#endif
				return true; 
			}
		}
		//fine roba schifosa 

		NewHookLocation = GrapplingHookComponent->GetHookLocation() + ReductionDirection * GetHookSpeed() * DeltaTime;
		GrapplingHookComponent->SetHookLocation(NewHookLocation);
			
		if (IsMinExtensionReached(DeltaTime))
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
	return false; // continue search mode tick
}

void UGHTargetTowardSelf::OrientGrabbedActorRotationToMovement() const 
{
	const FVector MoveDirection = (OwnerCharacter->GetActorLocation() - GetTargetGrabPoint()->Execute_GetLocation(GetTargetGrabPoint()->_getUObject())).GetSafeNormal();
	FRotator MovementRotation = MoveDirection.Rotation();
	MovementRotation.Pitch = 0.f;
	GetTargetGrabPoint()->Execute_GetActor(GetTargetGrabPoint()->_getUObject())->SetActorRotation(MovementRotation);
}

void UGHTargetTowardSelf::StopGrabOnLastValidTarget()
{
	if (LastValidTarget != nullptr)
	{
		LastValidTarget->Execute_OnStopGrab(LastValidTarget->_getUObject());
		LastValidTarget = nullptr;
		StopGrabOnLastValidTargetTimerHandle.Invalidate();
	}
}
