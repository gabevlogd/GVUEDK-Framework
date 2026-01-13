// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveCompletionModes/KillTarget/KillTarget.h"

#include "WaveSystem/WaveManager.h"
#include "WaveSystem/WaveCompletionModes/KillTarget/KillTargetComponent.h"

bool UKillTarget::Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager)
{
	WaveManager = InWaveManager;
	bTargetSpawned = false;
	bCallCompletionCheckOnEnemyDeath = true;
	UKillTargetComponent::OnRegisterTarget.AddUniqueDynamic(this, &UKillTarget::RegisterTarget);
	UKillTargetComponent::OnUnregisterTarget.AddUniqueDynamic(this, &UKillTarget::UnregisterTarget);
	return true;
}

void UKillTarget::BeginDestroy()
{
	UKillTargetComponent::OnRegisterTarget.RemoveDynamic(this, &UKillTarget::RegisterTarget);
	UKillTargetComponent::OnUnregisterTarget.RemoveDynamic(this, &UKillTarget::UnregisterTarget);
	Super::BeginDestroy();
}

bool UKillTarget::IsWaveComplete()
{
	if (!bTargetSpawned) return false;

	if (ActiveTargets.Num() != 0) return false;

	return true;
}

void UKillTarget::RegisterTarget(AActor* Target)
{
	if (!IsValid(Target)) return;
	bTargetSpawned = true;
	ActiveTargets.AddUnique(Target);
}

void UKillTarget::UnregisterTarget(AActor* Target)
{
	if (!IsValid(Target)) return;
	if (!ActiveTargets.Contains(Target)) return;
	ActiveTargets.Remove(Target);

	if (!WaveManager->HasPendingAsyncSpawns())
	{
		WaveManager->CheckWaveCompletion();
	}
		
}
