// Copyright Villains, Inc. All Rights Reserved.


#include "Utility/FMODSoundtrackUtility.h"

UFMODSoundtrackSubsystem* UFMODSoundtrackUtility::SoundtrackSubsystem = nullptr;

void UFMODSoundtrackUtility::Initialize(UFMODSoundtrackSubsystem* InSoundtrackSubsystem)
{
	if (SoundtrackSubsystem)
	{
		UE_LOG(LogFMODSoundtrackSubsystem, Warning, TEXT("FMODSoundtrackUtility: SoundtrackSubsystem is already initialized."));
		return;
	}
	
	if (!InSoundtrackSubsystem)
	{
		UE_LOG(LogFMODSoundtrackSubsystem, Warning, TEXT("FMODSoundtrackUtility: InSoundtrackSubsystem is null."));
		return;
	}
	
	SoundtrackSubsystem = InSoundtrackSubsystem;
}

void UFMODSoundtrackUtility::PlaySoundtrack(const FGameplayTag SoundtrackTag, const float FadeDuration, bool bPerformFade)
{
	INIT_CHECK();
	
	SoundtrackSubsystem->PlaySoundtrack(SoundtrackTag, FadeDuration, bPerformFade);
}

void UFMODSoundtrackUtility::StopSoundtrack(const float FadeOutTime, bool bFadeOut)
{
	INIT_CHECK();

	SoundtrackSubsystem->StopSoundtrack(FadeOutTime, bFadeOut);
}

bool UFMODSoundtrackUtility::SetCurrentSoundtrackParameter(const FName ParameterName, const float Value)
{
	if (!Initialized()) return false;
	return SoundtrackSubsystem->SetCurrentSoundtrackParameter(ParameterName, Value);
}

bool UFMODSoundtrackUtility::IsPlaying()
{
	if (!Initialized()) return false;
	return SoundtrackSubsystem->IsPlaying();
}

bool UFMODSoundtrackUtility::Initialized()
{
	return SoundtrackSubsystem != nullptr;
}
