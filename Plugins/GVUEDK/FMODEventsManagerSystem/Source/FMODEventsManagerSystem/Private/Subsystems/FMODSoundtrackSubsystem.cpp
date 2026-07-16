// Copyright Villains, Inc. All Rights Reserved.


#include "Subsystems/FMODSoundtrackSubsystem.h"
#include "Utility/FMODSoundtrackUtility.h"

void UFMODSoundtrackSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UE_LOG(LogFMODSoundtrackSubsystem, Display, TEXT("FMODSoundtrackSubsystem: Initializing..."));
	
	Settings = GetDefault<UFMODEventsSettings>();
	
	if (!Settings)
	{
		UE_LOG(LogFMODSoundtrackSubsystem, Error, TEXT("FMODSoundtrackSubsystem: SoundtrackSettings is not valid."));
		return;
	}
	
	if (const UFMODEventsRegister* InFMODEventsRegister = Settings->FMODEventsRegister.LoadSynchronous())
	{
		GameSoundtracks = InFMODEventsRegister->GameSoundtracks;
	}
	else
	{
		UE_LOG(LogFMODSoundtrackSubsystem, Error, TEXT("FMODSoundtrackSubsystem: SoundtrackRegister is not valid."));
		return;
	}

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UFMODSoundtrackSubsystem::OnPostWorldInitialization);
}

void UFMODSoundtrackSubsystem::Deinitialize()
{
	if (MusicChannelA) MusicChannelA->Stop();
	if (MusicChannelB) MusicChannelB->Stop();
	MusicChannelA = MusicChannelB = nullptr;

	UFMODSoundtrackUtility::Deinitialize();

	PendingPlayRequests.Empty();
	
	Super::Deinitialize();
}

void UFMODSoundtrackSubsystem::OnPostWorldInitialization(UWorld* World,
	FWorldInitializationValues WorldInitializationValues)
{
	World->OnWorldBeginPlay.AddUObject(this, &UFMODSoundtrackSubsystem::OnWorldBeginPlay);
}

void UFMODSoundtrackSubsystem::OnWorldBeginPlay()
{
	AActor* AudioActor = GetWorld()->SpawnActor<AActor>();
	MusicChannelA = CreateMusicComponent(AudioActor);
	MusicChannelB = CreateMusicComponent(AudioActor);

	ActiveChannel = MusicChannelA;

	UFMODSoundtrackUtility::Initialize(this);
}

UFMODAudioComponent* UFMODSoundtrackSubsystem::CreateMusicComponent(AActor* Owner)
{
	UFMODAudioComponent* Comp = NewObject<UFMODAudioComponent>(Owner);
	Comp->bAutoDestroy = false;
	Comp->RegisterComponent();
	Comp->SetVolume(0.f);
	return Comp;
}

void UFMODSoundtrackSubsystem::PlaySoundtrack(const FGameplayTag SoundtrackTag, float InFadeDuration, bool InPerformFade)
{
	if (!GameSoundtracks.Contains(SoundtrackTag))
	{
		UE_LOG(LogFMODSoundtrackSubsystem, Warning, TEXT("FMODSoundtrackSubsystem: SoundtrackTag %s not found."), *SoundtrackTag.ToString());
		return;
	}
	
	UFMODEvent* NewEvent = GameSoundtracks[SoundtrackTag];
	
	if (!NewEvent)
	{
		UE_LOG(LogFMODSoundtrackSubsystem, Warning, TEXT("FMODSoundtrackSubsystem: SoundtrackEvent for tag %s is null."), *SoundtrackTag.ToString());
		return;
	}

	// Avoid restarting the same event
	if (CurrentEvent == NewEvent && ActiveChannel && ActiveChannel->IsPlaying())
	{
		UE_LOG(LogFMODSoundtrackSubsystem, Warning, TEXT("FMODSoundtrackSubsystem: SoundtrackEvent for tag %s is already playing."), *SoundtrackTag.ToString());
		return;
	}

	// Avoid overlapping fades
	if (bFading)
	{
		UE_LOG(LogFMODSoundtrackSubsystem, Display, TEXT("FMODSoundtrackSubsystem: Currently fading, queuing soundtrack event for tag %s."), *SoundtrackTag.ToString());
		AddPlayRequest(SoundtrackTag, InFadeDuration, InPerformFade);
		return;
	}

	InactiveChannel = (ActiveChannel == MusicChannelA) ? MusicChannelB : MusicChannelA;

	// Setup the new event on the inactive channel
	InactiveChannel->SetEvent(NewEvent);
	InactiveChannel->Play();
	
	if (InPerformFade)
	{
		FadeDuration = FMath::Max(0.1f, InFadeDuration);
		FadeElapsed = 0.f;
		bFading = InPerformFade;
		InactiveChannel->SetVolume(0.f);	
	}
	else
	{
		ActiveChannel->Stop();
		InactiveChannel->SetVolume(1.f);
	}

	// Swap active and inactive channels
	UFMODAudioComponent* Temp = ActiveChannel;
	ActiveChannel = InactiveChannel;
	InactiveChannel = Temp;
	
	CurrentEvent = NewEvent;
	
	
	UE_LOG(LogFMODSoundtrackSubsystem, Display, TEXT("FMODSoundtrackSubsystem: Playing SoundtrackEvent for tag %s."), *SoundtrackTag.ToString());
}

void UFMODSoundtrackSubsystem::StopSoundtrack(float InFadeDuration, bool bFadeOut)
{
	if (ActiveChannel && ActiveChannel->IsPlaying())
	{
		if (bFadeOut && !bFading)
		{
			FadeDuration = FMath::Max(0.1f, InFadeDuration);
			FadeElapsed = 0.f;
			bFading = true;

			UFMODAudioComponent* Temp = ActiveChannel;
			ActiveChannel = InactiveChannel;
			InactiveChannel = Temp;
		}
		else
		{
			ActiveChannel->Stop();
			if (InactiveChannel && InactiveChannel->IsPlaying())
			{
				InactiveChannel->Stop();
				bFading = false;
			}
		}
		CurrentEvent = nullptr;
	}
}

void UFMODSoundtrackSubsystem::AddPlayRequest(const FGameplayTag SoundtrackTag, float InFadeDuration,
	bool InPerformFade)
{
	for (auto Element : PendingPlayRequests)
	{
		if (Element.SoundtrackTag == SoundtrackTag)
		{
			// Already in the queue
			return;
		}
	}
	PendingPlayRequests.Add(FPendingPlayRequest(SoundtrackTag, InFadeDuration, InPerformFade));
}

void UFMODSoundtrackSubsystem::ProcessPendingPlayRequests()
{
	if (PendingPlayRequests.Num() == 0) return;

	FPendingPlayRequest Request = PendingPlayRequests[0];
	PendingPlayRequests.RemoveAt(0);

	PlaySoundtrack(Request.SoundtrackTag, Request.FadeDuration, Request.bPerformFade);
}

bool UFMODSoundtrackSubsystem::SetCurrentSoundtrackParameter(const FName ParameterName, const float Value) const
{
	if (ActiveChannel && ActiveChannel->IsPlaying())
	{
		ActiveChannel->SetParameter(ParameterName, Value);
		return true;
	}
	return false;
}

bool UFMODSoundtrackSubsystem::IsPlaying() const
{
	return ActiveChannel && ActiveChannel->IsPlaying();
}

void UFMODSoundtrackSubsystem::Tick(float DeltaTime)
{
	if (!bFading) return;
	if (!IsValid(ActiveChannel) || !IsValid(InactiveChannel)) return;

	const float ActiveChannelVolume = FMath::Lerp(0.f, 1.f, FadeElapsed / FadeDuration);
	ActiveChannel->SetVolume(ActiveChannelVolume);
	InactiveChannel->SetVolume(1 - ActiveChannelVolume);
	FadeElapsed += DeltaTime;
	if (FadeElapsed >= FadeDuration || ActiveChannelVolume >= 1.f)
	{
		InactiveChannel->Stop();
		ActiveChannel->SetVolume(1.f);
		bFading = false;
		ProcessPendingPlayRequests();
	}
}
