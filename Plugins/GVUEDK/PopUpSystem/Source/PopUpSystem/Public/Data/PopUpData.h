// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "PopUpData.generated.h"

USTRUCT(BlueprintType)
struct FPopUpData
{
	GENERATED_BODY()

	/**
	 * Widget class to spawn for this pop up.
	 * IMPORTANT: This widget needs to implement the IPopUp interface to work properly
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data")
	TSubclassOf<UUserWidget> WidgetClass;

	/**
	 * Gameplay tag to identify this pop up, used to add/remove/show/hide the pop up through the PopUpManagerSubsystem
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data")
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data")
	bool bCustomizeTitleStyle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data", meta = (EditCondition = "bCustomizeTitleStyle", EditConditionHides))
	FSlateFontInfo TitleFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data", meta = (EditCondition = "bCustomizeTitleStyle", EditConditionHides))
	TEnumAsByte<ETextJustify::Type> TitleJustification;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data", meta = (MultiLine = "true"))
	FString Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data")
	bool bCustomizeMessageStyle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data", meta = (MultiLine = "true", EditCondition = "bCustomizeMessageStyle", EditConditionHides))
	FSlateFontInfo MessageFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data", meta = (EditCondition = "bCustomizeMessageStyle", EditConditionHides))
	TEnumAsByte<ETextJustify::Type> MessageJustification;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data", meta = (EditCondition = "bCustomizeMessageStyle", EditConditionHides))
	FMargin MessagePadding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopUp Data", AdvancedDisplay)
	TMap<FString, UObject*> Parameters;

	bool IsValid() const;
};
