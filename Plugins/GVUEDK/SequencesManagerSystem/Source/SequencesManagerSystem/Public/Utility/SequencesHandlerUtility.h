// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystems/SequencesHandlerSubsystem.h"
#include "SequencesHandlerUtility.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCESMANAGERSYSTEM_API USequencesHandlerUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static void Initialize(USequencesHandlerSubsystem* InSequencesHandler);

	UFUNCTION(BlueprintCallable)
	static bool PlaySequence(ULevelSequence* SequenceToPlay, FMovieSceneSequencePlaybackSettings PlaybackSettings, ALevelSequenceActor*& OutSequenceActor);

private:

	static bool Initialized();

private:

	static USequencesHandlerSubsystem* SequencesHandler;
};
