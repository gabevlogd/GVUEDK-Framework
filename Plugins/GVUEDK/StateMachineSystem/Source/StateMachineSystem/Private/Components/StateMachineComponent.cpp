// Copyright (c) 2025, Gabevlogd. All rights reserved.


#include "Components/StateMachineComponent.h"

#include "NativeGameplayTags.h"
#include "Components/MultiStateMachineComponent.h"
#include "Net/UnrealNetwork.h"

// Debug
#if !UE_BUILD_SHIPPING

enum class EDebugStateMachine : uint8
{
	Off = 0,
	Screen = 1,
	Log = 2,
	All = 3
};

static TAutoConsoleVariable<int32> CVarDebug(
	TEXT("sms.Debug"),
	0,
	TEXT("State Machine Debug\n")
	TEXT("0 = Off\n")
	TEXT("1 = On\n"),
	ECVF_Default);

static TAutoConsoleVariable<int32> CVarDebugMode(
	TEXT("sms.DebugMode"),
	1,
	TEXT("State Machine Debug Mode\n")
	TEXT("1 = Screen\n")
	TEXT("2 = Log\n")
	TEXT("3 = All"),
	ECVF_Default);

static TAutoConsoleVariable<FString> CVarDebugFilter(
	TEXT("sms.DebugFilter"),
	TEXT(""),
	TEXT("Filter by Actor name"),
	ECVF_Default);

static EDebugStateMachine GetDebugMode()
{
	return static_cast<EDebugStateMachine>(
		CVarDebugMode.GetValueOnGameThread());
}

static bool DebugEnabled()
{
	return CVarDebug.GetValueOnGameThread() != 0;
}

static bool ShouldDebug(const UStateMachineComponent* Comp)
{
	if (!DebugEnabled())
	{
		return false;
	}
	
	const FString Filter = CVarDebugFilter.GetValueOnGameThread();

	const AActor* Owner = Comp->GetOwner();
	return Owner && Owner->GetName().Contains(Filter);
}

static void PrintDebugInfo(const UStateMachineComponent* Comp)
{
	if (!ShouldDebug(Comp))
	{
		return;
	}
	
	switch (GetDebugMode())
	{
	case EDebugStateMachine::Screen:
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(TEXT("%s: %s"), *Comp->GetStateMachineTag().ToString(), *Comp->GetCurrentStateTag().ToString()));
		break;
	case EDebugStateMachine::Log:
		UE_LOG(LogMultiStateMachine, Log, TEXT("%s: %s"), *Comp->GetStateMachineTag().ToString(), *Comp->GetCurrentStateTag().ToString());
		break;
	case EDebugStateMachine::All:
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(TEXT("%s: %s"), *Comp->GetStateMachineTag().ToString(), *Comp->GetCurrentStateTag().ToString()));
		UE_LOG(LogMultiStateMachine, Log, TEXT("%s: %s"), *Comp->GetStateMachineTag().ToString(), *Comp->GetCurrentStateTag().ToString());
		
		break;
	default:
		break;
	}
}

#endif

UStateMachineComponent::UStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsRunning = false;
	bInitialized = false;
	MultiStateMachine = nullptr;
	EntryStateClass = nullptr;
	CurrentState = nullptr;
	PreviousState = nullptr;
	Context = nullptr;
	EntryState = nullptr;
	bPaused = false;
	ReplicatedData = FReplicatedData();
}

void UStateMachineComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStateMachineComponent, ReplicatedData);
}

UStateBase* UStateMachineComponent::ChangeState(const FGameplayTag NextState)
{
	if (!bInitialized || bPaused) return nullptr;

	UStateBase* NextStateInstance = nullptr; 
	if (TryGetState(NextState, NextStateInstance))
	{
		if (/*NextStateInstance->GetIsRunning()*/NextState == CurrentState->GetStateTag())
		{
#if !UE_BUILD_SHIPPING
			if (DebugEnabled())
				UE_LOG(LogStateMachine, Warning, TEXT("%s already running."), *NextState.ToString());
#endif
			return nullptr;
		}
		
		//if this state machine is running under a multi-state machine,
		//we will check the negators of the next state to know if it can be loaded
		if (IsValid(MultiStateMachine))
		{
			if (IsNegated(NextStateInstance))
			{
				return nullptr;
			}
		}
		
		ChangeState(NextStateInstance);

		// After changing the state, we will check if this new state has to interrupt other states
		// of other state machines in the same multi-state machine
		if (IsValid(MultiStateMachine))
		{
			MultiStateMachine->CheckStateToInterrupt(NextStateInstance);
		}
	}
	return NextStateInstance;
}

void UStateMachineComponent::ChangeState(UStateBase* NextState)
{
	PreviousState = CurrentState;
	CurrentState->Exit(NextState->GetStateTag());
	CurrentState = NextState;
	CurrentState->Enter();
	if (bReplicateData && GetIsReplicated())
	{
		Server_ReplicateData(FReplicatedData(
			CurrentState->GetStateTag(),
			PreviousState->GetStateTag(),
			StateMachineTag));
	}
	OnStateChanged.Broadcast(GetStateMachineTag(), PreviousState->GetStateTag(), CurrentState->GetStateTag());
}

bool UStateMachineComponent::TryGetState(const FGameplayTag StateTag, UStateBase*& OutState) const
{
	if (bInitialized)
	{
		if (StatesMap.Contains(StateTag))
		{
			OutState = StatesMap[StateTag];
			return true;
		}
		OutState = nullptr;
		UE_LOG(LogStateMachine, Warning, TEXT("State Tag: %s not found"), *StateTag.ToString());
		return false;
	}
	OutState = nullptr;
	UE_LOG(LogStateMachine, Warning, TEXT("Component not initialized"));
	return false;
}

UStateBase* UStateMachineComponent::GetState(const FGameplayTag StateTag) const
{
	if (bInitialized)
	{
		if (StatesMap.Contains(StateTag))
		{
			return StatesMap[StateTag];
		}
		UE_LOG(LogStateMachine, Warning, TEXT("State Tag: %s not found"), *StateTag.ToString());
		return nullptr;
	}
	UE_LOG(LogStateMachine, Warning, TEXT("Component not initialized"));
	return nullptr;
}

void UStateMachineComponent::SetConfigData(const UStateMachineConfig* StateMachineData)
{
	if (!IsValid(StateMachineData))
	{
		UE_LOG(LogStateMachine, Warning, TEXT("SetConfigData: Invalid State Machine Data"));
		return;
	}
	
	StateMachineTag = StateMachineData->StateMachineTag;
	EntryStateClass = StateMachineData->EntryStateClass;
	StateClasses = StateMachineData->StateClasses;
	bReplicateData = StateMachineData->bReplicateData;
}

void UStateMachineComponent::Initialize()
{
	Context = GetOwner();

	//States initialization
	StatesMap = TMap<FGameplayTag, UStateBase*>();
	
	if (IsValid(EntryStateClass))
	{
		EntryState = NewObject<UStateBase>(this, EntryStateClass);
		EntryState->Initialize(Context, this);
		StatesMap.Add(EntryState->StateTag, EntryState);
		UE_LOG(LogStateMachine, Log, TEXT("State %s created"), *EntryState->GetName());
	}
	else
	{
		UE_LOG(LogStateMachine, Warning, TEXT("Missing EntryState, state machine will not work"));
		bInitialized = false;
		return;
	}
	
	for (auto Element : StateClasses)
	{
		if (!IsValid(Element))
		{
			UE_LOG(LogStateMachine, Warning, TEXT("Invalid State"));
			continue;
		}
		UStateBase* StateInstance = NewObject<UStateBase>(this, Element);
		if (StatesMap.Contains(StateInstance->GetStateTag()))
		{
			UE_LOG(LogStateMachine, Warning, TEXT("State Tag: %s already exists, state: %s will not work"), *StateInstance->GetStateTag().ToString(), *StateInstance->GetName());
			continue;
		}
		StateInstance->Initialize(Context, this);
		StatesMap.Add(StateInstance->GetStateTag(), StateInstance);
		UE_LOG(LogStateMachine, Log, TEXT("State %s created"), *StateInstance->GetName());
	}
	
	bInitialized = !StatesMap.IsEmpty();

	// if state machine initialized correctly run the entry point
	if (bInitialized)
	{
		bIsRunning = true;
		CurrentState = EntryState;
		CurrentState->Enter();
	}
}

bool UStateMachineComponent::IsNegated(const UStateBase* StateToCheck) const
{
	for (auto Element : MultiStateMachine->StateMachinesMap)
	{
		// Skip if the element is the same state machine as the current one
		if (Element.Key == GetStateMachineTag())
		{
			continue;
		}
		for (auto Tag : StateToCheck->Negators)
		{
			if (Tag == Element.Value->GetCurrentStateTag())
			{
#if !UE_BUILD_SHIPPING
				if (DebugEnabled())
				UE_LOG(LogStateMachine, Log, TEXT("%s cannot run because it is negated by %s in state machine %s"), 
				*StateToCheck->GetStateTag().ToString(), *Element.Value->GetCurrentState()->GetStateTag().ToString(), *Element.Key.ToString());
#endif
				return true;
			}
		}
	}
	return false;
}

void UStateMachineComponent::OnRep_ReplicatedData()
{
	OnReplicatedDataChanged.Broadcast(ReplicatedData);
}

void UStateMachineComponent::Server_ReplicateData_Implementation(const FReplicatedData& InReplicatedData)
{
	// Update server-side value
	ReplicatedData = InReplicatedData;
	OnReplicatedDataChanged.Broadcast(ReplicatedData);
}

void UStateMachineComponent::HandleInput(const FGameplayTag InputActionTag, const FInputActionValue& Value)
{
	if (!bInitialized || bPaused)
	{
		return;
	}
	
	CurrentState->HandleInput(InputActionTag, Value);
}

void UStateMachineComponent::InterruptCurrentState(const FGameplayTag Interrupter)
{
	if (!bInitialized)
	{
		return;
	}

	if (CurrentState == EntryState)
	{
		UE_LOG(LogStateMachine, Warning, TEXT("Trying to interrupt entry state with interrupter %s, this should not happen, check your state machine logic"), *Interrupter.ToString());
		return;
	}

	CurrentState->Interrupt(Interrupter);
	ChangeState(EntryState);
}

void UStateMachineComponent::Pause(const bool bResetToEntryState)
{
	bPaused = true;
	if (bResetToEntryState)
	{
		InterruptCurrentState(FGameplayTag::EmptyTag);
	}
}

void UStateMachineComponent::BeginPlay()
{
	Initialize();
	Super::BeginPlay();
}

void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bInitialized || bPaused)
	{
		return;
	}
	
	CurrentState->Update(DeltaTime);

#if !UE_BUILD_SHIPPING
	PrintDebugInfo(this);
#endif
}
