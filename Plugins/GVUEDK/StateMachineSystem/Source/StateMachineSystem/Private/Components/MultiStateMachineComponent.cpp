// Copyright (c) 2025, Gabevlogd. All rights reserved.


#include "Components/MultiStateMachineComponent.h"

#include "Engine/ActorChannel.h"


UMultiStateMachineComponent::UMultiStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bInitialized = false;
	bPaused = false;
	SetIsReplicatedByDefault(true);
}

bool UMultiStateMachineComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool Result = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (const auto Element : StateMachinesMap)
	{
		UStateMachineComponent* StateMachine = Element.Value;
		if (IsValid(StateMachine) && StateMachine->WantsReplication())
		{
			Result |= Channel->ReplicateSubobject(StateMachine, *Bunch, *RepFlags);
		}
	}
	return Result;
}

void UMultiStateMachineComponent::ChangeState(const FGameplayTag NextState, const FGameplayTag RelativeStateMachine)
{
	if (!bInitialized || bPaused)
	{
		return;
	}

	if (!StateMachinesMap.Contains(RelativeStateMachine))
	{
		UE_LOG(LogMultiStateMachine, Warning, TEXT("ChangeState: Invalid State Machine tag"));
		return;
	}
	
	StateMachinesMap[RelativeStateMachine]->ChangeState(NextState);
}

void UMultiStateMachineComponent::HandleInput(const FGameplayTag InputActionTag, const FInputActionValue& Value, const FGameplayTag RelativeStateMachine)
{
	if (!bInitialized || bPaused)
	{
		return;
	}

	if (!StateMachinesMap.Contains(RelativeStateMachine))
	{
		UE_LOG(LogMultiStateMachine, Warning, TEXT("HandleInput: Invalid state machine tag"));
		return;
	}
	
	StateMachinesMap[RelativeStateMachine]->HandleInput(InputActionTag, Value);
}

void UMultiStateMachineComponent::PauseAll(const bool bResetToEntryState)
{
	for (const auto Element : StateMachinesMap)
	{
		Element.Value->Pause(bResetToEntryState);
	}
	bPaused = true;
}

void UMultiStateMachineComponent::Pause(FGameplayTag StateMachineTag, const bool bResetToEntryState)
{
	if (!StateMachinesMap.Contains(StateMachineTag))
	{
		UE_LOG(LogMultiStateMachine, Warning, TEXT("Pause: Invalid state machine tag"));
		return;
	}

	StateMachinesMap[StateMachineTag]->Pause(bResetToEntryState);
}

void UMultiStateMachineComponent::UnPauseAll()
{
	for (const auto Element : StateMachinesMap)
	{
		Element.Value->UnPause();
	}
	bPaused = false;
}

void UMultiStateMachineComponent::UnPause(FGameplayTag StateMachineTag)
{
	if (!StateMachinesMap.Contains(StateMachineTag))
	{
		UE_LOG(LogMultiStateMachine, Warning, TEXT("UnPause: Invalid state machine tag"));
		return;
	}

	StateMachinesMap[StateMachineTag]->UnPause();
}

void UMultiStateMachineComponent::CheckStateToInterrupt(const UStateBase* Interrupter)
{
	for (auto Element : StateMachinesMap)
	{
		// Check if the state machine is the same as the one of the interrupter
		if (Element.Key == Interrupter->GetRelativeStateMachine()->GetStateMachineTag())
        {
            continue;
        }

		if (Element.Value->GetCurrentState()->Interrupters.Contains(Interrupter->GetStateTag()))
		{
			Element.Value->InterruptCurrentState(Interrupter->GetStateTag());
		}
	}
}

void UMultiStateMachineComponent::Initialize()
{
	StateMachinesMap = TMap<FGameplayTag, UStateMachineComponent*>();
	for (const auto Element : StateMachines)
	{
		AddStateMachine(Element);
	}

	bInitialized = !StateMachinesMap.IsEmpty();
}

void UMultiStateMachineComponent::AddStateMachine(UStateMachineConfig* StateMachineData)
{
	if (!IsValid(StateMachineData))
	{
		UE_LOG(LogMultiStateMachine, Warning, TEXT("AddStateMachine: Invalid State Machine found"));
		return;
	}
		
	if (StateMachinesMap.Contains(StateMachineData->StateMachineTag))
	{
		UE_LOG(LogMultiStateMachine, Warning, TEXT("State Machine Tag: %s already exists, associated state machine will not work"), *StateMachineData->StateMachineTag.ToString());
		return;
	}

	FName SMName = FName(*FString::Printf(TEXT("SM_%s"), *StateMachineData->StateMachineTag.ToString()));
	UStateMachineComponent* StateMachine = NewObject<UStateMachineComponent>(this, SMName);
	StateMachine->SetConfigData(StateMachineData);
	StateMachinesMap.Add(StateMachineData->StateMachineTag, StateMachine);
	StateMachine->OnStateChanged.AddDynamic(this, &UMultiStateMachineComponent::StateChanged);
	StateMachine->OnReplicatedDataChanged.AddDynamic(this, &UMultiStateMachineComponent::ReplicatedDataChanged);
	StateMachine->MultiStateMachine = this;
	if (StateMachine->WantsReplication())
	{
		StateMachine->SetIsReplicated(true);
		StateMachine->SetNetAddressable(); 
		if (GetOwner()->HasAuthority())
		{
			AddReplicatedSubObject(StateMachine);
		}
	}
	StateMachine->RegisterComponent();
	// This avoids the engine to call TickComponent on the state machine component and allows us to control when it ticks by calling its TickComponent function directly from the MultiStateMachineComponent tick
	StateMachine->SetComponentTickEnabled(false); 
	UE_LOG(LogMultiStateMachine, Log, TEXT("State Machine %s added to state machines map"), *StateMachine->GetName());
}

FReplicatedData UMultiStateMachineComponent::GetReplicatedData(FGameplayTag StateMachineTag) const
{
	if (StateMachinesMap.Contains(StateMachineTag))
	{
		return StateMachinesMap[StateMachineTag]->GetReplicatedData();
	}
	
	UE_LOG(LogMultiStateMachine, Warning, TEXT("GetReplicatedData: Invalid State Machine tag"));
	return FReplicatedData();
}

void UMultiStateMachineComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bInitialized)
	{
		return;
	}
	
	for (const auto Element : StateMachinesMap)
	{
		Element.Value->TickComponent(DeltaTime, TickType, ThisTickFunction);
	}
}

void UMultiStateMachineComponent::StateChanged(FGameplayTag StateMachineTag, FGameplayTag PreviousState,
	FGameplayTag NewState)
{
	OnStateChanged.Broadcast(StateMachineTag, PreviousState, NewState);
}

void UMultiStateMachineComponent::ReplicatedDataChanged(const FReplicatedData& NewData)
{
	OnReplicatedDataChanged.Broadcast(NewData);
}

