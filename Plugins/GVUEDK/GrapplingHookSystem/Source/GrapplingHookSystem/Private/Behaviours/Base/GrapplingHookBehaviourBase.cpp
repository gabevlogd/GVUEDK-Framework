// Copyright Villains, Inc. All Rights Reserved.


#include "Behaviours/Base/GrapplingHookBehaviourBase.h"

#include "Components/GrapplingHookComponent.h"

void UGrapplingHookBehaviourBase::Initialize(UGrapplingHookComponent* InGrapplingHookComponent)
{
	GrapplingHookComponent = InGrapplingHookComponent;
	OwnerCharacter = InGrapplingHookComponent->GetOwnerCharacter();
}

void UGrapplingHookBehaviourBase::EnterMode()
{
	GrapplingHookComponent->SetHookHitObstacle(false);
	GrapplingHookComponent->SetTargetGrabPoint(nullptr);
}

void UGrapplingHookBehaviourBase::ExitMode()
{
	GrapplingHookComponent->SetHookHitObstacle(false);
	GrapplingHookComponent->SetTargetGrabPoint(nullptr);
}

IGrabPoint* UGrapplingHookBehaviourBase::GetTargetGrabPoint() const
{
	return GrapplingHookComponent->GetTargetGrabPoint();
}

void UGrapplingHookBehaviourBase::SetTargetGrabPoint(IGrabPoint* GrabPoint)
{
	GrapplingHookComponent->SetTargetGrabPoint(GrabPoint);
}

bool UGrapplingHookBehaviourBase::IsTargetAcquired() const
{
	return GrapplingHookComponent->IsTargetAcquired();
}
