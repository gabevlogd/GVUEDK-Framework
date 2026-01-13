// Copyright Villains, Inc. All Rights Reserved.


#include "Utility/SequencesHandlerUtility.h"

USequencesHandlerSubsystem* USequencesHandlerUtility::SequencesHandler = nullptr;

void USequencesHandlerUtility::Initialize(USequencesHandlerSubsystem* InSequencesHandler)
{
	if (!IsValid(InSequencesHandler))
	{
		UE_LOG(LogSequencesManagerSystem, Warning, TEXT("SequencesHandlerUtility::Initialize: InSequencesHandler is not valid"));
		return;
	}

	SequencesHandler = InSequencesHandler;
}

bool USequencesHandlerUtility::PlaySequence(ULevelSequence* SequenceToPlay,
	FMovieSceneSequencePlaybackSettings PlaybackSettings, ALevelSequenceActor*& OutSequenceActor)
{
	if (!Initialized())
	{
		UE_LOG(LogSequencesManagerSystem, Warning, TEXT("SequencesHandlerUtility::PlaySequence: SequencesHandler is not initialized"));
		return false;
	}
	
	return SequencesHandler->PlaySequence(SequenceToPlay, PlaybackSettings, OutSequenceActor);
}

bool USequencesHandlerUtility::Initialized()
{
	return IsValid(SequencesHandler);
}
