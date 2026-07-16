// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Settings/SequencesManagerSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequencesHandlerSubsystem.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogSequencesManagerSystem, All, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSequenceSkipped);

/**
 * 
 */
UCLASS()
class SEQUENCESMANAGERSYSTEM_API USequencesHandlerSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

private:

	friend class USequencesHandlerUtility;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	bool PlaySequence(ULevelSequence* SequenceToPlay, const FMovieSceneSequencePlaybackSettings& PlaybackSettings, ALevelSequenceActor*& OutSequenceActor);

	UFUNCTION()
	void OnSequenceStarted();

	UFUNCTION()
	void OnSequenceStopped();
	
	// FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return bTickEnabled; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UFMODSoundtrackSubsystem, STATGROUP_Tickables); }
	virtual bool IsTickableWhenPaused() const override { return false; }
	virtual bool IsTickableInEditor() const override { return false; }
	// End of FTickableGameObject interface

private:

	UPROPERTY(BlueprintAssignable)
	FOnSequenceSkipped OnSequenceSkipped;

	UPROPERTY()
	const USequencesManagerSettings* Settings;
	
	UPROPERTY()
	ULevelSequencePlayer* CurrentSequencePlayer;

	UPROPERTY()
	TSubclassOf<UUserWidget> SkipSequenceWidgetClass;

	UPROPERTY()
	UUserWidget* SkipSequenceWidgetInstance;

	FKey KeyboardSkipKey = EKeys::SpaceBar;
	
	FKey GamepadSkipKey = EKeys::Gamepad_FaceButton_Bottom;

	float SkipKeyHoldTime = 2.f;

	float SkipKeyStartTime = 0.f;

	int SkipSequenceWidgetZOrder = 1000;
	
	bool bTickEnabled = false;

	bool bAddWidgetIndicator = false;

	

};
