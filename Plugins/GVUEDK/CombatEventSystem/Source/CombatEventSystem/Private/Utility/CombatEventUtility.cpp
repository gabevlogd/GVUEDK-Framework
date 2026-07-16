// Copyright Villains, Inc. All Rights Reserved.


#include "Utility/CombatEventUtility.h"

UCombatEventSubsystem* UCombatEventUtility::CombatEventSubsystem = nullptr;

void UCombatEventUtility::Initialize(UCombatEventSubsystem* InCombatEventSubsystem)
{
	if (!IsValid(InCombatEventSubsystem))
	{
		UE_LOG(LogCombatEventSystem, Error, TEXT("UCombatEventUtility::Initialize - InCombatEventSubsystem is null."));
		CombatEventSubsystem = nullptr;
		return;
	}

	CombatEventSubsystem = InCombatEventSubsystem;
}

void UCombatEventUtility::NotifyCombatDeathEvent(const FCombatDeathEvent& CombatDeathEvent)
{
	if (!IsValid(CombatEventSubsystem))
	{
		UE_LOG(LogCombatEventSystem, Error, TEXT("UCombatEventUtility::NotifyCombatDeathEvent - CombatEventSubsystem is not initialized."));
		return;
	}

	CombatEventSubsystem->OnDeathEvent.Broadcast(CombatDeathEvent);
}
