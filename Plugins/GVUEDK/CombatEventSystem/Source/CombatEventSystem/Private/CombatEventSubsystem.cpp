// Copyright Villains, Inc. All Rights Reserved.


#include "CombatEventSubsystem.h"

#include "Utility/CombatEventUtility.h"

void UCombatEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UCombatEventUtility::Initialize(this);
}
