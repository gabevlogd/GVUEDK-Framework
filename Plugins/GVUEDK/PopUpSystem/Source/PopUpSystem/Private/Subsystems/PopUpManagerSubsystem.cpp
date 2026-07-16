// Copyright Villains, Inc. All Rights Reserved.


#include "Subsystems/PopUpManagerSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Interfaces/PopUp.h"
#include "Utility/PopUpSystemUtility.h"

void UPopUpManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogPopUpManagerSubsystem, Display, TEXT("PopUpManagerSubsystem: Initializing..."));

	// Get the PopUpSystemSettings
	PopUpSystemSettings = GetDefault<UPopUpSystemSettings>();
	
	if (UPopUpRegister* InPopUpRegister = PopUpSystemSettings->PopUpRegister.LoadSynchronous())
	{
		PopUpDataMap = InPopUpRegister->GetPopUpDataMap();
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
	if (PopUpDataMap.Num() == 0)
	{
		UE_LOG(LogPopUpManagerSubsystem, Error, TEXT("PopUpManagerSubsystem: No PopUpData found in PopUpDataMap. Make sure to register a valid PopUpRegister in PopUpSystemSettings."));
		return nullptr;
	}

	if (!PopUpDataMap.Contains(PopUpTag))
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
	const TSubclassOf<UUserWidget> WidgetClass = PopUpDataMap[PopUpTag].WidgetClass;
	const FName WidgetName = FName(*PopUpTag.ToString());
	
	if (UUserWidget* PopUpWidget = CreateWidget<UUserWidget, APlayerController*>(WidgetOwner, WidgetClass, WidgetName))
	{
		PopUpWidget->AddToViewport();
		ActivePopUps.Add(PopUpTag, PopUpWidget);
		IPopUp::Execute_InitPopUp(PopUpWidget, PopUpDataMap[PopUpTag]);
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

	IPopUp::Execute_RemovePopUp(ActivePopUps[PopUpTag]);
	ActivePopUps.Remove(PopUpTag);
}

void UPopUpManagerSubsystem::RemoveAllPopUps()
{
	TArray<FGameplayTag> Keys;
	ActivePopUps.GetKeys(Keys);

	for (const auto& Key : Keys)
	{
		RemovePopUp(Key);
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
