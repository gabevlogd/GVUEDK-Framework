// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveCompletionModes/KillTarget/KillTargetComponent.h"

FRegisterTarget UKillTargetComponent::OnRegisterTarget;
FUnregisterTarget UKillTargetComponent::OnUnregisterTarget;

UKillTargetComponent::UKillTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UKillTargetComponent::RegisterTarget(AActor* Target)
{
	OnRegisterTarget.Broadcast(Target);
}

void UKillTargetComponent::UnregisterTarget(AActor* Target)
{
	OnUnregisterTarget.Broadcast(Target);
}


