// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveCompletionModes/GlobalTimerMode.h"

#include "WaveSystem/WaveManager.h"

FTimerHandle UGlobalTimerMode::GlobalTimerHandle = FTimerHandle();

bool UGlobalTimerMode::Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager)
{
	if (!InWaveManager->GetWorld()->GetTimerManager().IsTimerActive(GlobalTimerHandle))
	{
		if (!Super::Init(Template, InWaveManager)) return false;
		GlobalTimerHandle = TimerHandle;
	}
	else
	{
		WaveManager = InWaveManager;
		const float RemainingGlobalTimer = WaveManager->GetWorld()->GetTimerManager().GetTimerRemaining(GlobalTimerHandle);
		WaveManager->GetWorld()->GetTimerManager().ClearTimer(GlobalTimerHandle);
		if (RemainingGlobalTimer > 0.f)
		{
			GlobalTimerHandle = FTimerHandle();
			WaveManager->GetWorld()->GetTimerManager().SetTimer(GlobalTimerHandle, this, &UGlobalTimerMode::TimerCompleted, RemainingGlobalTimer, false);
			bCallCompletionCheckOnEnemyDeath = false;
		}
		else 
		{
			bCallCompletionCheckOnEnemyDeath = true;
		}
		WaveManager->OnWaveCanceled.AddUniqueDynamic(this, &UGlobalTimerMode::Deinitialize);
	}
	WaveManager->OnWaveCanceled.AddUniqueDynamic(this, &UGlobalTimerMode::InvalidateGlobalTimer);

	// if call completion check on enemy death is ture it means the global timer has already completed
	// so we can skip setting up the current wave timer and directly check for wave completion based on enemy deaths
	if (bCallCompletionCheckOnEnemyDeath) return true;
	
	if (const UGlobalTimerMode* GlobalTimerTemplate = Cast<UGlobalTimerMode>(Template))
	{
		CurrentWaveCompletionTime = GlobalTimerTemplate->CurrentWaveCompletionTime;
	}
	else
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("UGlobalTimerMode::Init - Template is not of type UGlobalTimerMode"));
		return false;
	}

	if (CurrentWaveCompletionTime > 0.f)
	{
		CurrentWaveTimerHandle = FTimerHandle();
		WaveManager->GetWorld()->GetTimerManager().SetTimer(CurrentWaveTimerHandle, this, &UGlobalTimerMode::CurrentWaveTimerCompleted, CurrentWaveCompletionTime, false);
	}
	else
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("UGlobalTimerMode::Init - CurrentWaveCompletionTime is not set or invalid"));
	}

	return true;
}

bool UGlobalTimerMode::IsWaveComplete()
{
	if (bCallCompletionCheckOnEnemyDeath)
	{
		return WaveManager->GetGlobalAliveEnemiesCount() <= 0;
	}
	if (bCurrentWaveTimerCompleted)
	{
		return true;
	}
	return false;
}

float UGlobalTimerMode::GetWaveRemainingTime() const
{
	return IsValid(WaveManager) && IsValid(WaveManager->GetWorld()) ?
	WaveManager->GetWorld()->GetTimerManager().GetTimerRemaining(CurrentWaveTimerHandle) :
	-1.f;
}

float UGlobalTimerMode::GetAllWaveRemainingTime() const
{
	return IsValid(WaveManager) && IsValid(WaveManager->GetWorld()) ?
	WaveManager->GetWorld()->GetTimerManager().GetTimerRemaining(GlobalTimerHandle) :
	-1.f;
}

bool UGlobalTimerMode::HasNextWave() const
{
	return WaveManager->GetWorld()->GetTimerManager().IsTimerActive(GlobalTimerHandle);
}

void UGlobalTimerMode::CurrentWaveTimerCompleted()
{
	bCurrentWaveTimerCompleted = true;
	WaveManager->CheckWaveCompletion();
}

void UGlobalTimerMode::TimerCompleted()
{
	bCallCompletionCheckOnEnemyDeath = true;
	WaveManager->CallCompletionCheckOnEnemyDeath();
	GlobalTimerHandle.Invalidate();
	WaveManager->GetWorld()->GetTimerManager().ClearTimer(CurrentWaveTimerHandle);
	CurrentWaveTimerHandle.Invalidate();
	WaveManager->CheckWaveCompletion();
}

void UGlobalTimerMode::Deinitialize(const UWaveData* WaveData, const int32 WaveIndex)
{
	if (IsValid(WaveManager) && IsValid(WaveManager->GetWorld()))
	{
		WaveManager->OnWaveCanceled.RemoveDynamic(this, &UGlobalTimerMode::Deinitialize);
		WaveManager->OnWaveCanceled.RemoveDynamic(this, &UGlobalTimerMode::InvalidateGlobalTimer);
		TimerHandle.Invalidate();
		WaveManager->GetWorld()->GetTimerManager().ClearTimer(CurrentWaveTimerHandle);
		CurrentWaveTimerHandle.Invalidate();
		bCurrentWaveTimerCompleted = false;
	}
}

void UGlobalTimerMode::InvalidateGlobalTimer(const UWaveData* WaveData, const int32 WaveIndex)
{
	if (IsValid(WaveManager) && IsValid(WaveManager->GetWorld()))
	{
		WaveManager->OnWaveCanceled.RemoveDynamic(this, &UGlobalTimerMode::InvalidateGlobalTimer);
		WaveManager->GetWorld()->GetTimerManager().ClearTimer(GlobalTimerHandle);
		GlobalTimerHandle.Invalidate();
	}
}
