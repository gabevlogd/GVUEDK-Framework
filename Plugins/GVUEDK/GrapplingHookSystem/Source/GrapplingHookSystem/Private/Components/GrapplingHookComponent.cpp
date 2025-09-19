// Copyright Villains, Inc. All Rights Reserved.


#include "Components/GrapplingHookComponent.h"

#include "Behaviours/MovementBehaviours/Base/GHMovementBehaviourBase.h"
#include "Behaviours/SearchBehaviours/Base/GHSearchBehaviourBase.h"
#include "GameFramework/Character.h"


UGrapplingHookComponent::UGrapplingHookComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bTargetAcquired = false;
	bInitialized = false;
	TargetGrabPoint = nullptr;
	OwnerCharacter = nullptr;
}

void UGrapplingHookComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("%s is not a character, %s need to be on a character to work properly"), *GetOwner()->GetName(), *GetName());
		bInitialized = false;
		return;
	}
	bInitialized = InitializeModes();
}

void UGrapplingHookComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	if (!bInitialized)
	{
		return;
	}
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentMode.MovementBehaviour->TickMode(DeltaTime))
	{
		return;
	}
	CurrentMode.SearchBehaviour->TickMode(DeltaTime);
}

void UGrapplingHookComponent::ChangeMode(const FGameplayTag ModeTag)
{
	if (ModeMap.Contains(ModeTag))
	{
		CurrentMode.MovementBehaviour->ExitMode();
		CurrentMode.SearchBehaviour->ExitMode();
		CurrentMode = ModeMap[ModeTag];
		CurrentMode.MovementBehaviour->EnterMode();
		CurrentMode.SearchBehaviour->EnterMode();
		OnModeChanged.Broadcast(ModeTag);
	}
	else UE_LOG(LogTemp, Warning, TEXT("Trying change mode with invalid mode tag: %s"), *ModeTag.ToString());
	
}

bool UGrapplingHookComponent::InitializeModes()
{
	ModeMap = TMap<FGameplayTag, FGrapplingHookMode>();
	for (auto Element : Modes)
	{
		// Check if the mode tag is already used
		if (ModeMap.Contains(Element.ModeTag))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s already used as a mode tag"), *Element.ModeTag.ToString());
			continue;
		}

		// Check if the movement and search behaviours are valid
		if (!IsValid(Element.MovementBehaviour) || !IsValid(Element.SearchBehaviour))
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Movement or Search Behaviour for %s"), *Element.ModeTag.ToString());
			continue;
		}

		Element.MovementBehaviour->Initialize(this);
		Element.SearchBehaviour->Initialize(this);
		ModeMap.Add(Element.ModeTag, Element);
	}

	// if there is at least one mode, set the current mode to the first one
	if (ModeMap.Num() > 0)
	{
		CurrentMode = ModeMap[ModeMap.CreateConstIterator()->Key];
		CurrentMode.MovementBehaviour->EnterMode();
		CurrentMode.SearchBehaviour->EnterMode();
		return true;
	}
	return false;
}
