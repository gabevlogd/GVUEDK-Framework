// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SequencesManagerData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SEQUENCESMANAGERSYSTEM_API USequencesManagerData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAddSkipSequenceWidgetIndicator = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bAddSkipSequenceWidgetIndicator"))
	TSubclassOf<UUserWidget> SkipSequenceWidgetIndicator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bAddSkipSequenceWidgetIndicator"))
	int SkipSequenceWidgetZOrder = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bAddSkipSequenceWidgetIndicator"))
	FKey KeyboardSkipKey = EKeys::SpaceBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bAddSkipSequenceWidgetIndicator"))
	FKey GamepadSkipKey = EKeys::Gamepad_FaceButton_Bottom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bAddSkipSequenceWidgetIndicator"))
	float SkipKeyHoldTime = 2.f;
};
