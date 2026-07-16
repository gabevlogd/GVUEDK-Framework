// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystems/FMODSoundtrackSubsystem.h"
#include "FMODSoundtrackUtility.generated.h"


#define INIT_CHECK() \
do { \
if (!Initialized()) { \
UE_LOG(LogFMODSoundtrackSubsystem, Warning, TEXT("FMODSoundtrackUtility: SoundtrackSubsystem is not initialized.")); \
return; \
} \
} while(0)

/**
 * 
 */
UCLASS()
class FMODEVENTSMANAGERSYSTEM_API UFMODSoundtrackUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:

	static void Initialize(UFMODSoundtrackSubsystem* InSoundtrackSubsystem);
	static void Deinitialize() { SoundtrackSubsystem = nullptr; }

	UFUNCTION(BlueprintCallable)
	static void PlaySoundtrack(const FGameplayTag SoundtrackTag, float FadeDuration = 1.5f, bool bPerformFade = true);

	UFUNCTION(BlueprintCallable)
	static void StopSoundtrack(float FadeOutTime = 1.0f, bool bFadeOut = true);

	UFUNCTION(BlueprintCallable)
	static bool SetCurrentSoundtrackParameter(const FName ParameterName, const float Value);

	UFUNCTION(BlueprintCallable)
	static bool IsPlaying();

private:

	static bool Initialized();
	
private:
	
	static UFMODSoundtrackSubsystem* SoundtrackSubsystem;
};
