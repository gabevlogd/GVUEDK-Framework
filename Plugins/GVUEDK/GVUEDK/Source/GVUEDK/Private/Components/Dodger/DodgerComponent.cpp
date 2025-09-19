// Copyright Villains, Inc. All Rights Reserved.


#include "Components/Dodger/DodgerComponent.h"

#include <string>

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utility/FGvDebug.h"


UDodgerComponent::UDodgerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDodgerComponent::StartDodge(FVector WorldDirection)
{
	if (!bInitialized || bIsCoolingDown) return;
	
	bIsDodging = true;
	ElapsedTime = 0.f;
	
	if (bDirectionalDodge)
	{
		WorldDirection = WorldDirection.IsNearlyZero() ? GetDefaultDirection() : WorldDirection.GetSafeNormal();
	}
	else WorldDirection = GetDefaultDirection();

	TargetVelocity = WorldDirection * GetSpeed();
	
	if (bApplyGravity) TargetVelocity.Z -= GRAVITY * GravityScale;

	if (bOrientRotationToMovement)
	{
		TargetRotation = FRotator(Owner->GetActorRotation().Pitch, WorldDirection.Rotation().Yaw, Owner->GetActorRotation().Roll);
		CurrentRotation = Owner->GetActorRotation();
	}
	
	Owner->GetCharacterMovement()->StopMovementImmediately();

	StartCooldown();
	
	OnStartDodge.Broadcast();
}

void UDodgerComponent::StopDodge()
{
	if (!bInitialized) return;
	if (!bConserveMomentum)
	{
		Owner->GetCharacterMovement()->StopMovementImmediately();
	}
	bIsDodging = false;
	OnStopDodge.Broadcast();
}

void UDodgerComponent::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}

void UDodgerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	if (!bInitialized) return;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsDodging)
	{
		ElapsedTime += DeltaTime;
		if (ElapsedTime <= StartDelay)
		{
			return;
		}
		PerformDodge(DeltaTime);
	}
}

void UDodgerComponent::Initialize()
{
	Owner = Cast<ACharacter>(GetOwner());
	if (!Owner)
	{
		bInitialized = false;
		FGvDebug::Warning(GetOwner()->GetName() + " is not a Character, can't have a DodgerComponent", true);
		return;
	}

	if (!bApplyUniformVelocity && !IsValid(SpeedCurve))
	{
		FGvDebug::Warning("SpeedCurve is not valid, setting bApplyLinearVelocity to false", true);
		bApplyUniformVelocity = true;
	}

	InitDefaultDirection();

	bInitialized = true;
}

void UDodgerComponent::PerformDodge(float DeltaTime)
{
	
	if (bOrientRotationToMovement)
	{
		CurrentRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
		Owner->SetActorRotation(CurrentRotation);
	}
	
	if (!bApplyUniformVelocity)
	{
		TargetVelocity = TargetVelocity.GetSafeNormal() * GetSpeed();
		if (bApplyGravity)
        {
            TargetVelocity.Z -= GRAVITY * GravityScale;
        }
	}

	Owner->GetCharacterMovement()->Velocity = TargetVelocity;
	
	OnDodge.Broadcast();
	if (ElapsedTime >= DodgeDuration + StartDelay)
	{
		StopDodge();
	}
}

void UDodgerComponent::InitDefaultDirection()
{
	switch (DefaultDirection)
	{
	case EDefaultDirection::CharacterForward:
		RelativeDirectionTransform = &GetOwner()->GetTransform();
		DefaultLocalDirection = FVector::ForwardVector;
		break;
	case EDefaultDirection::CharacterBackward:
		RelativeDirectionTransform = &GetOwner()->GetTransform();
		DefaultLocalDirection = FVector::BackwardVector;
		break;
	case EDefaultDirection::CharacterLeft:
		RelativeDirectionTransform = &GetOwner()->GetTransform();
		DefaultLocalDirection = FVector::LeftVector;
		break;
	case EDefaultDirection::CharacterRight:
		RelativeDirectionTransform = &GetOwner()->GetTransform();
		DefaultLocalDirection = FVector::RightVector;
		break;
	case EDefaultDirection::SightForward:
		RelativeDirectionTransform = &GetOwner()->FindComponentByClass<UCameraComponent>()->GetComponentTransform();
		DefaultLocalDirection = FVector::ForwardVector;
		break;
	case EDefaultDirection::SightBackward:
		RelativeDirectionTransform = &GetOwner()->FindComponentByClass<UCameraComponent>()->GetComponentTransform();
		DefaultLocalDirection = FVector::BackwardVector;
		break;
	case EDefaultDirection::SightLeft:
		RelativeDirectionTransform = &GetOwner()->FindComponentByClass<UCameraComponent>()->GetComponentTransform();
		DefaultLocalDirection = FVector::LeftVector;
		break;
	case EDefaultDirection::SightRight:
		RelativeDirectionTransform = &GetOwner()->FindComponentByClass<UCameraComponent>()->GetComponentTransform();
		DefaultLocalDirection = FVector::RightVector;
		break;
	}

	if (RelativeDirectionTransform == nullptr)
	{
		FGvDebug::Warning("Default direction based on sight requires a camera on " + GetOwner()->GetName(), true);
		RelativeDirectionTransform = &GetOwner()->GetTransform();
	}
}

void UDodgerComponent::StartCooldown()
{
	if (const UWorld* World = GetWorld())
	{
		FTimerDelegate TimerDel;
		TimerDel.BindLambda([this]
		{
			bIsCoolingDown = false;
			OnDodgeCooldownComplete.Broadcast();
		});
		
		FTimerHandle TimerHandle;
		
		World->GetTimerManager().SetTimer(TimerHandle, TimerDel, DodgeCooldown, false);

		//if the DodgeCooldown <= 0 the timer does not start so we do not want bIsCoolingDown to become true
		bIsCoolingDown = DodgeCooldown > 0;
	}
}

float UDodgerComponent::GetSpeed() const
{
	if (bApplyUniformVelocity)
	{
		return LinearSpeed;
	}
	return SpeedCurve->GetFloatValue(ElapsedTime / (DodgeDuration + StartDelay)) * 100.f; // Convert to cm/s
}

FVector UDodgerComponent::GetDefaultDirection() const
{
	return RelativeDirectionTransform->TransformVectorNoScale(DefaultLocalDirection).GetSafeNormal();
}
