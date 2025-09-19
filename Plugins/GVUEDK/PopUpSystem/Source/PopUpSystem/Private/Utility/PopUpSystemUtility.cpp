// Copyright Villains, Inc. All Rights Reserved.


#include "Utility/PopUpSystemUtility.h"

UPopUpManagerSubsystem* UPopUpSystemUtility::PopUpManager = nullptr;

void UPopUpSystemUtility::Init(UPopUpManagerSubsystem* InPopUpManager)
{
	PopUpManager = InPopUpManager;
}

UUserWidget* UPopUpSystemUtility::AddPopUp(const FGameplayTag PopUpTag)
{
	if (PopUpManager)
	{
		return PopUpManager->AddPopUp(PopUpTag);
	}
	return nullptr;
}

void UPopUpSystemUtility::RemovePopUp(const FGameplayTag PopUpTag)
{
	if (PopUpManager)
	{
		PopUpManager->RemovePopUp(PopUpTag);
	}
}

void UPopUpSystemUtility::RemoveAllPopUps()
{
	if (PopUpManager)
	{
		PopUpManager->RemoveAllPopUps();
	}
}

void UPopUpSystemUtility::HidePopUp(const FGameplayTag PopUpTag)
{
	if (PopUpManager)
	{
		PopUpManager->HidePopUp(PopUpTag);
	}
}

void UPopUpSystemUtility::ShowPopUp(const FGameplayTag PopUpTag)
{
	if (PopUpManager)
	{
		PopUpManager->ShowPopUp(PopUpTag);
	}
}

void UPopUpSystemUtility::HideAllPopUps()
{ 
	if (PopUpManager)
	{
		PopUpManager->HideAllPopUps();
	}
}

void UPopUpSystemUtility::ShowAllPopUps()
{ 
	if (PopUpManager)
	{
		PopUpManager->ShowAllPopUps();
	}
}

bool UPopUpSystemUtility::TryGetActivePopUp(const FGameplayTag PopUpTag, UUserWidget*& OutPopUpWidget)
{
	if (PopUpManager)
	{
		return PopUpManager->TryGetActivePopUp(PopUpTag, OutPopUpWidget);
	}
	return false;
}

bool UPopUpSystemUtility::IsPopUpActive(const FGameplayTag PopUpTag)
{
	if (PopUpManager)
	{
		return PopUpManager->IsPopUpActive(PopUpTag);
	}
	return false;
}
