// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveCompletionModes/CustomCompletionMode/CustomCompletionMode.h"
#include "WaveSystem/WaveManager.h"

bool UCustomCompletionMode::Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager)
{
	WaveManager = InWaveManager;

	if (const UCustomCompletionMode* CustomModeTemplate = Cast<UCustomCompletionMode>(Template))
	{
		CompletionConditionClass = CustomModeTemplate->CompletionConditionClass;
		MetaClass = CustomModeTemplate->MetaClass;
		bCallCompletionCheckOnEnemyDeath = CustomModeTemplate->bCompletionCheckOnEnemyDeath;
	}
	else
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("UCustomCompletionMode::Init - Template is not of type UCustomCompletionMode"));
		return false;
	}

	if (!IsValid(CompletionConditionClass))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("UCustomCompletionMode::Init - CompletionConditionClass is not valid"));
		return false;
	}
	CompletionConditionInstance = NewObject<UCompletionCondition>(this, CompletionConditionClass);
	CompletionConditionInstance->Init(WaveManager);
	
	if (IsValid(MetaClass))
	{
		Meta = NewObject<UObject>(this, MetaClass);
	}
	
	return true;
}

bool UCustomCompletionMode::IsWaveComplete()
{
	const UWaveData* CurrentWaveData = WaveManager->GetCurrentWaveData();
	const int32 CurrentWaveIndex = WaveManager->GetCurrentWaveIndex();

	return CompletionConditionInstance->IsWaveComplete(WaveManager, CurrentWaveData, CurrentWaveIndex, Meta);
}
