// Copyright Villains, Inc. All Rights Reserved.


#include "Subsystems/PopUpManagerSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Utility/PopUpSystemUtility.h"

void UPopUpManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogPopUpManagerSubsystem, Display, TEXT("PopUpManagerSubsystem: Initializing..."));

	// Get the PopUpSystemSettings
	PopUpSystemSettings = GetDefault<UPopUpSystemSettings>();
	
	if (UPopUpRegister* InPopUpRegister = PopUpSystemSettings->PopUpRegister.LoadSynchronous())
	{
		PopUpRegister = InPopUpRegister;
		UE_LOG(LogPopUpManagerSubsystem, Display, TEXT("PopUpManagerSubsystem: Registered PopUpRegister: %s"), *PopUpRegister->GetName());
	}
	else
	{
		UE_LOG(LogPopUpManagerSubsystem, Error, TEXT("PopUpManagerSubsystem: PopUpRegister is not valid."));
		return;
	}

	ActivePopUps = TMap<FGameplayTag, UUserWidget*>();

	UPopUpSystemUtility::Init(this);
}

UUserWidget* UPopUpManagerSubsystem::AddPopUp(const FGameplayTag& PopUpTag)
{
	if (!PopUpRegister)
	{
		UE_LOG(LogPopUpManagerSubsystem, Error, TEXT("PopUpManagerSubsystem: PopUpRegister is not valid."));
		return nullptr;
	}

	if (!PopUpRegister->PopUpRegisterItems.Contains(PopUpTag))
	{
		UE_LOG(LogPopUpManagerSubsystem, Error, TEXT("PopUpManagerSubsystem: PopUpTag %s is not registered."), *PopUpTag.ToString());
		return nullptr;
	}
	
	if (ActivePopUps.Contains(PopUpTag))
	{
		UE_LOG(LogPopUpManagerSubsystem, Warning, TEXT("PopUpManagerSubsystem: PopUp %s already added."), *PopUpTag.ToString());
		return nullptr;
	}

	APlayerController* WidgetOwner = GetWorld()->GetFirstPlayerController();
	const TSubclassOf<UUserWidget> WidgetClass = PopUpRegister->PopUpRegisterItems[PopUpTag];
	const FName WidgetName = FName(*PopUpTag.ToString());
	
	if (UUserWidget* PopUpWidget = CreateWidget<UUserWidget, APlayerController*>(WidgetOwner, WidgetClass, WidgetName))
	{
		PopUpWidget->AddToViewport();
		ActivePopUps.Add(PopUpTag, PopUpWidget);
		return PopUpWidget;
	}
	else
	{
		UE_LOG(LogPopUpManagerSubsystem, Error, TEXT("PopUpManagerSubsystem: Failed to create PopUp %s."), *PopUpTag.ToString());
		return nullptr;
	}
}

void UPopUpManagerSubsystem::RemovePopUp(const FGameplayTag& PopUpTag)
{
	if (!ActivePopUps.Contains(PopUpTag))
	{
		UE_LOG(LogPopUpManagerSubsystem, Warning, TEXT("PopUpManagerSubsystem: PopUp %s not found."), *PopUpTag.ToString());
		return;
	}

	ActivePopUps[PopUpTag]->RemoveFromParent();
	ActivePopUps.Remove(PopUpTag);
}

void UPopUpManagerSubsystem::RemoveAllPopUps()
{
	for (const auto Element : ActivePopUps)
	{
		RemovePopUp(Element.Key);
	}
}

void UPopUpManagerSubsystem::HidePopUp(const FGameplayTag& PopUpTag)
{
	if (!ActivePopUps.Contains(PopUpTag))
	{
		UE_LOG(LogPopUpManagerSubsystem, Warning, TEXT("PopUpManagerSubsystem: PopUp %s not found."), *PopUpTag.ToString());
		return;
	}

	ActivePopUps[PopUpTag]->SetVisibility(ESlateVisibility::Collapsed);
}

void UPopUpManagerSubsystem::ShowPopUp(const FGameplayTag& PopUpTag)
{
	if (!ActivePopUps.Contains(PopUpTag))
	{
		UE_LOG(LogPopUpManagerSubsystem, Warning, TEXT("PopUpManagerSubsystem: PopUp %s not found."), *PopUpTag.ToString());
		return;
	}

	ActivePopUps[PopUpTag]->SetVisibility(ESlateVisibility::Visible);
}

void UPopUpManagerSubsystem::HideAllPopUps()
{
	for (const auto Element : ActivePopUps)
	{
		HidePopUp(Element.Key);
	}
}

void UPopUpManagerSubsystem::ShowAllPopUps()
{
	for (const auto Element : ActivePopUps)
	{
		ShowPopUp(Element.Key);
	}
}

bool UPopUpManagerSubsystem::TryGetActivePopUp(const FGameplayTag& PopUpTag, UUserWidget*& OutPopUpWidget) const
{
	OutPopUpWidget = nullptr;
	
	if (!ActivePopUps.Contains(PopUpTag))
	{
		UE_LOG(LogPopUpManagerSubsystem, Warning, TEXT("PopUpManagerSubsystem: PopUp %s not found."), *PopUpTag.ToString());
		return false;
	}

	OutPopUpWidget = ActivePopUps[PopUpTag];
	return true;
}

bool UPopUpManagerSubsystem::IsPopUpActive(const FGameplayTag& PopUpTag) const
{
	return ActivePopUps.Contains(PopUpTag);
}
