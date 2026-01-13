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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FKey KeyboardSkipKey = EKeys::SpaceBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FKey GamepadSkipKey = EKeys::Gamepad_FaceButton_Bottom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SkipKeyHoldTime = 2.f;
};
