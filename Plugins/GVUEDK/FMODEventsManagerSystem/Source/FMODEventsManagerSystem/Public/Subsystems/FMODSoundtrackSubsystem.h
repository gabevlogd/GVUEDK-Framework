// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODEvent.h"
#include "FMODAudioComponent.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Settings/FMODEventsSettings.h"
#include "FMODSoundtrackSubsystem.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogFMODSoundtrackSubsystem, All, All);

struct FPendingPlayRequest
{
	FPendingPlayRequest()
	{
		SoundtrackTag = FGameplayTag::EmptyTag;
		FadeDuration = 1.5f;
		bPerformFade = true;
	}

	FPendingPlayRequest(FGameplayTag InSoundtrackTag, float InFadeDuration, bool InPerformFade)
	{
		SoundtrackTag = InSoundtrackTag;
		FadeDuration = InFadeDuration;
		bPerformFade = InPerformFade;
	}
	
	FGameplayTag SoundtrackTag;
	float FadeDuration;
	bool bPerformFade;
};

/**
 * 
 */
UCLASS()
class FMODEVENTSMANAGERSYSTEM_API UFMODSoundtrackSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

	friend class UFMODSoundtrackUtility;

private:
	
	UPROPERTY()
	const UFMODEventsSettings* Settings = nullptr;
	
	UPROPERTY()
	TMap<FGameplayTag, UFMODEvent*> GameSoundtracks;
	
	UPROPERTY(Transient)
	UFMODAudioComponent* MusicChannelA = nullptr;

	UPROPERTY(Transient)
	UFMODAudioComponent* MusicChannelB = nullptr;

	UPROPERTY(Transient)
	UFMODAudioComponent* ActiveChannel = nullptr;

	UPROPERTY(Transient)
	UFMODAudioComponent* InactiveChannel = nullptr;
	
	UPROPERTY(Transient)
	UFMODEvent* CurrentEvent = nullptr;

	TArray<FPendingPlayRequest> PendingPlayRequests;

	bool bFading = false;
	float FadeDuration = 1.5f;
	float FadeElapsed = 0.f;

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;

	void OnPostWorldInitialization(UWorld* World, FWorldInitializationValues WorldInitializationValues);
	
	void OnWorldBeginPlay();
	
	UFMODAudioComponent* CreateMusicComponent(AActor* Owner);
	
	void PlaySoundtrack(const FGameplayTag SoundtrackTag, float InFadeDuration = 1.5f, bool InPerformFade = true);
	
	void StopSoundtrack(float InFadeDuration = 1.0f, bool bFadeOut = true);

	void AddPlayRequest(const FGameplayTag SoundtrackTag, float InFadeDuration = 1.5f, bool InPerformFade = true);

	void ProcessPendingPlayRequests();

	bool SetCurrentSoundtrackParameter(const FName ParameterName, const float Value) const;
	
	bool IsPlaying() const;

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return true; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UFMODSoundtrackSubsystem, STATGROUP_Tickables); }
	virtual bool IsTickableWhenPaused() const override { return false; }
	virtual bool IsTickableInEditor() const override { return false; }

};
