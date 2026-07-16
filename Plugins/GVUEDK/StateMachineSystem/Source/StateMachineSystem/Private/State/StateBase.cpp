// Copyright (c) 2025, Gabevlogd. All rights reserved.


#include "State/StateBase.h"
#include "TransitionEvents.h"
#include "Components/StateMachineComponent.h"


void UStateBase::Initialize(AActor* Context, UStateMachineComponent* StateMachine)
{
	if (!IsValid(Context))
	{
		bInitialized = false;
		UE_LOG(LogStateBase, Warning, TEXT("State %s: Context is not valid"), *GetName());
		return;
	}
	StateContext = Context;
	RelativeStateMachine = StateMachine;
	bInitialized = true;
	NativeOnInitialize(Context);
	OnInitialize(Context);
}

void UStateBase::Enter()
{
	if (!bInitialized)
	{
		return;
	}
	//UE_LOG(LogStateBase, Log, TEXT("State %s: Enter"), *GetName());
	bIsRunning = true;
	NativeOnEnter();
	OnEnter();
}

void UStateBase::Update(const float DeltaTime)
{
	if (!bInitialized)
	{
		return;
	}
	NativeOnUpdate(DeltaTime);
	OnUpdate(DeltaTime);
}

void UStateBase::Exit(const FGameplayTag& NextStateTag)
{
	if (!bInitialized)
	{
		return;
	}
	//UE_LOG(LogStateBase, Warning, TEXT("State %s: Exit"), *GetName());
	bIsRunning = false;
	NativeOnExit(NextStateTag);
	OnExit(NextStateTag);
}

void UStateBase::HandleInput(const FGameplayTag InputActionTag, const FInputActionValue& Value)
{
	if (!bInitialized)
	{
		return;
	}
	
	// Check if the input action tag is in the transitions map, if so, check the transition condition and if it is satisfied, run the transition events and change the state
	for (auto Element : Transitions)
	{
		if (Element.Key == InputActionTag)
		{
			if (IsValid(Element.Value.TransitionEvents))
			{
				if (!Element.Value.TransitionEvents->TransitionCondition(StateContext, InputActionTag, Value, this, Element.Value.ToState))
				{
					break;
				}
				Element.Value.TransitionEvents->OnTransition(StateContext, InputActionTag, Value, this, Element.Value.ToState);
			}
			RelativeStateMachine->ChangeState(Element.Value.ToState);
			return;
		}
	}

	// If the InputActionTag is not in the transitions map, call the native and blueprint handle input events
	NativeOnHandleInput(InputActionTag, Value);
	OnHandleInput(InputActionTag, Value);
}

void UStateBase::Interrupt(const FGameplayTag Interrupter)
{
	if (!bInitialized)
	{
		return;
	}
	
	NativeOnInterrupt(Interrupter);
	OnInterrupt(Interrupter);
}
