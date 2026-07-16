// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PopUp.generated.h"

struct FPopUpData;

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UPopUp : public UInterface
{
	GENERATED_BODY()
};

class POPUPSYSTEM_API IPopUp
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Popup")
	void InitPopUp(const FPopUpData& PopUpData);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Popup")
	void RemovePopUp();
};



