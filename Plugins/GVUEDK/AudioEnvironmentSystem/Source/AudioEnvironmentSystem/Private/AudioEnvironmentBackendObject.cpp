// Copyright Villains, Inc. All Rights Reserved.


#include "AudioEnvironmentBackendObject.h"

void UAudioEnvironmentBackendObject::OnListenerEnvironmentChanged(const FGuid& InListenerID,
	const FAudioEnvironmentListenerState& InPrevState, const FAudioEnvironmentListenerState& InNewState)
{
	BP_OnListenerEnvironmentChanged(InListenerID, InPrevState, InNewState);
}

void UAudioEnvironmentBackendObject::OnListenerEnvironmentTick(const FGuid& InListenerID,
	const FAudioEnvironmentListenerState& InCurrentState)
{
	BP_OnListenerEnvironmentTick(InListenerID, InCurrentState);
}

void UAudioEnvironmentBackendObject::Initialize()
{
	BP_Initialize();
}

void UAudioEnvironmentBackendObject::Shutdown()
{
	BP_Shutdown();
}
