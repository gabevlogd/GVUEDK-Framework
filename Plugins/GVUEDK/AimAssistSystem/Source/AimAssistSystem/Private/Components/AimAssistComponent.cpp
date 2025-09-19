// Copyright Villains, Inc. All Rights Reserved.


#include "Components/AimAssistComponent.h"

#include "KismetTraceUtils.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


UAimAssistComponent::UAimAssistComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	MaxDetectionDistance = 5000.f;
	MinDetectionDistance = 0.f;
	DetectionRadius = 100.f;
	AimAssistMinAngle = 45.f;
	AdjustAimForce = 100.f;
	TargetDetectionTime = 0.f;
	TargetLostTime = 0.f;
	LastAjustAimForceOut = 0.f;
	CurrentTarget = nullptr;
	PreviousTarget = nullptr;
	Controller = nullptr;
	CameraManager = nullptr;
	LastValidTarget = nullptr;
	AdjustAimCurve = nullptr;
	TargetsObjectChannel = ECC_Pawn;
	bHasTarget = false;
}

void UAimAssistComponent::EnableAimAssist()
{
	if (!IsValid(Controller))
	{
		Controller = Cast<APawn>(GetOwner())->GetController();
	}
	if (!IsValid(CameraManager))
	{
		CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	}
	TargetLostTime = 0.f;
	TargetDetectionTime = 0.f;
	CurrentTarget = nullptr;
	PreviousTarget = nullptr;
	bHasTarget = false;
	SetComponentTickEnabled(true);
}

void UAimAssistComponent::DisableAimAssist()
{
	SetComponentTickEnabled(false);
}

void UAimAssistComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdateCurrentTarget(DeltaTime);
	UpdatePreviousTarget(DeltaTime);

	if (bHasTarget)
	{
		TargetDetectionTime += DeltaTime;
	}
	else
	{
		TargetLostTime += DeltaTime;
	}

	if (IsValid(AdjustAimCurve))
	{
		AdjustAimSmooth(DeltaTime);
		return;
	}
	
	if (!IsValid(CurrentTarget)) return;
	
	AdjustAim(DeltaTime, CurrentTarget, AdjustAimForce);
}

void UAimAssistComponent::UpdateCurrentTarget(const float DeltaTime)
{
	TArray<FHitResult> HitResults; 
	
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(TargetsObjectChannel);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	const FVector StartLocation = CameraManager->GetCameraCacheView().Location + CameraManager->GetCameraCacheView().Rotation.Vector() * MinDetectionDistance;
	const FVector EndLocation = CameraManager->GetCameraCacheView().Location + CameraManager->GetCameraCacheView().Rotation.Vector() * MaxDetectionDistance;
	
	// Detects all possible targets 
	if (GetWorld()->SweepMultiByObjectType(HitResults, StartLocation, EndLocation, FQuat::Identity, ObjectParams,
	                                        FCollisionShape::MakeSphere(DetectionRadius), CollisionParams))
	{
		CurrentTarget = GetMostPointedTarget(HitResults, (EndLocation - StartLocation).GetSafeNormal(), StartLocation);
		
		if (IsValid(CurrentTarget))
		{
			// Check if there is an actor between the aim direction and the current target
			FHitResult HitResult;
			CollisionParams.AddIgnoredActor(CurrentTarget);
			if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, CurrentTarget->GetActorLocation(),
													 ECC_Visibility, CollisionParams))
			{
				if (HitResult.GetActor()->GetRootComponent()->GetCollisionObjectType() == TargetsObjectChannel)
				{
					CurrentTarget = HitResult.GetActor();
				}
			}
			LastValidTarget = CurrentTarget;
			bHasTarget = true;
		}
	}
	else
	{
		CurrentTarget = nullptr;
	}

# if !UE_BUILD_SHIPPING
	if (bShowDebug)
	{
		DrawDebugSweptSphere(GetWorld(), StartLocation, EndLocation, DetectionRadius, FColor::Green, false, -1);
		if (IsValid(CurrentTarget))
			DrawDebugSphere(GetWorld(), CurrentTarget->GetActorLocation(), 50.f, 12, FColor::Red, false, -1);
	}
#endif
}

void UAimAssistComponent::UpdatePreviousTarget(const float DeltaTime)
{
	if (PreviousTarget != CurrentTarget)
	{
		if (IsValid(CurrentTarget))
		{
			PreviousTarget = CurrentTarget;
			TargetDetectionTime = 0.f;
			OnNewTargetDetected.Broadcast(CurrentTarget);
		}
		else if (bHasTarget)
		{
			bHasTarget = false;
			TargetLostTime  = 0.f;
			OnTargetLost.Broadcast(PreviousTarget);
			PreviousTarget = nullptr;
		}
	}
}

void UAimAssistComponent::AdjustAim(const float DeltaTime, const AActor* TargetToAim, const float AimAdjustmentForce) const 
{
	const FVector EyeLocation = CameraManager->GetCameraCacheView().Location;
	
	FVector TargetLocation = FVector::ZeroVector;
	if (const ACharacter* TargetChar = Cast<ACharacter>(TargetToAim))
	{
		TargetLocation = TargetChar->GetMesh()->GetBoneLocation(BoneToAim);
		if (TargetLocation == FVector::ZeroVector)
		{
			TargetLocation = TargetChar->GetActorLocation();
		}
	}
	const FVector DirectionToTarget = TargetLocation - EyeLocation;
	const FRotator Current = Controller->GetControlRotation();
	const FRotator Target = DirectionToTarget.Rotation();
	const FRotator NewRotation = FMath::RInterpConstantTo(Current, Target, DeltaTime, AimAdjustmentForce);
	Controller->SetControlRotation(NewRotation);
}

void UAimAssistComponent::AdjustAimSmooth(const float DeltaTime) 
{
	if (IsValid(CurrentTarget))
	{
		if (LastAjustAimForceOut > 0.f)
		{
			AdjustAim(DeltaTime, CurrentTarget, FMath::Max(LastAjustAimForceOut, GetAdjustAimForceIn(TargetDetectionTime)));
		}
		else
		{
			AdjustAim(DeltaTime, CurrentTarget, GetAdjustAimForceIn(TargetDetectionTime));
		}
	}
	else if (IsValid(LastValidTarget))
	{
		const float AdjustAimForceOut = GetAdjustAimForceOut(TargetLostTime);
		if (AdjustAimForceOut <= 0.f)
		{
			LastAjustAimForceOut = 0.f;
			LastValidTarget = nullptr;
			return;
		}
		LastAjustAimForceOut = AdjustAimForceOut;
		AdjustAim(DeltaTime, LastValidTarget, AdjustAimForceOut);
	}
}

AActor* UAimAssistComponent::GetMostPointedTarget(const TArray<FHitResult>& Targets, const FVector& AimDirection,
                                                  const FVector& EyeLocation) const
{
	if (Targets.Num() == 0) return nullptr;

	AActor* MostPointedTarget = nullptr;
	float MinAngle = FMath::Cos(FMath::DegreesToRadians(AimAssistMinAngle));
	for (const auto Target : Targets)
	{
		const FVector TargetLocation = Target.GetActor()->GetActorLocation();
		const FVector DirectionToTarget = (TargetLocation - EyeLocation).GetSafeNormal();
		const float Angle = FVector::DotProduct(DirectionToTarget, AimDirection);
		if (Angle > MinAngle)
		{
			MinAngle = Angle;
			MostPointedTarget = Target.GetActor();
		}
	}
	return MostPointedTarget;
}

float UAimAssistComponent::GetAdjustAimForceIn(const float Time) const
{
	return AdjustAimCurve->GetVectorValue(Time).X;
}

float UAimAssistComponent::GetAdjustAimForceOut(const float Time) const
{
	return AdjustAimCurve->GetVectorValue(Time).Y;
}
