// Copyright Villains, Inc. All Rights Reserved.


#include "Components/InputBindingComponent.h"
#include "EnhancedInputSubsystems.h"


UInputBindingComponent::UInputBindingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	InputUserSettings = nullptr;
}

void UInputBindingComponent::Initialize()
{
	InputUserSettings = GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->GetUserSettings();
	for (const auto Element : InputMappingContexts)
	{
		if (!IsMappingContextRegistered(Element))
		{
			RegisterInputMappingContext(Element);
		}
	}

	//GetCurrentKeyProfile()->GetPlayerMappableKeySettings(PlayerMappableKeySettings);
}

void UInputBindingComponent::BeginPlay()
{
	Initialize();
	Super::BeginPlay();
}

