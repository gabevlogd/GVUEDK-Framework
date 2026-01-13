// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveCompletionModes/TimerMode.h"
#include "WaveSystem/WaveManager.h"

bool UTimerMode::Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager)
{
	WaveManager = InWaveManager;
	bCallCompletionCheckOnEnemyDeath = false;

	if (const UTimerMode* TimerModeTemplate = Cast<UTimerMode>(Template))
	{
		TimerDuration = TimerModeTemplate->TimerDuration;
	}
	else
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("UTimerMode::Init - Template is not of type UTimerMode"));
		return false;
	}

	TimerHandle = FTimerHandle();
	WaveManager->GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTimerMode::TimerCompleted, TimerDuration, false);
	WaveManager->OnWaveCanceled.AddUniqueDynamic(this, &UTimerMode::Deinitialize);

	return true;
}

void UTimerMode::Deinitialize(const UWaveData* WaveData, const int32 WaveIndex)
{
	if (IsValid(WaveManager) && IsValid(WaveManager->GetWorld()))
	{
		WaveManager->OnWaveCanceled.RemoveDynamic(this, &UTimerMode::Deinitialize);
		WaveManager->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
}

bool UTimerMode::IsWaveComplete()
{
	return bTimerCompleted;
}

float UTimerMode::GetWaveRemainingTime() const
{
	return IsValid(WaveManager) && IsValid(WaveManager->GetWorld()) ?
	WaveManager->GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle) :
	-1.f;
}

void UTimerMode::BeginDestroy()
{
	Deinitialize(nullptr, 0);
	Super::BeginDestroy();
}

void UTimerMode::TimerCompleted()
{
	bTimerCompleted = true;
	WaveManager->CheckWaveCompletion();
}

