// Copyright Villains, Inc. All Rights Reserved.


#include "Subsystems/SequencesHandlerSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Utility/SequencesHandlerUtility.h"

void USequencesHandlerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Settings = GetDefault<USequencesManagerSettings>();
	
	if (!Settings)
	{
		UE_LOG(LogSequencesManagerSystem , Error, TEXT("SequencesHandlerSubsystem: Settings is not valid."));
		return;
	}

	if (const USequencesManagerData* SettingsData = Settings->SettingsData.LoadSynchronous())
	{
		KeyboardSkipKey = SettingsData->KeyboardSkipKey;
		GamepadSkipKey = SettingsData->GamepadSkipKey;
		SkipKeyHoldTime = FMath::Max(SettingsData->SkipKeyHoldTime, 0.01f);
		bAddWidgetIndicator = SettingsData->bAddSkipSequenceWidgetIndicator;
		bAddWidgetIndicator ? SkipSequenceWidgetClass = SettingsData->SkipSequenceWidgetIndicator : nullptr;
	}
	else
	{
		UE_LOG(LogSequencesManagerSystem , Error, TEXT("SequencesHandlerSubsystem: Settings Data is not valid."));
		return;
	}

	
	USequencesHandlerUtility::Initialize(this);
}

bool USequencesHandlerSubsystem::PlaySequence(ULevelSequence* SequenceToPlay, const FMovieSceneSequencePlaybackSettings& PlaybackSettings, ALevelSequenceActor*& OutSequenceActor)
{
	UWorld* World = GetWorld();
	
	if (!IsValid(World) || !IsValid(SequenceToPlay))
	{
		return false;
	}

	CurrentSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(World, SequenceToPlay, PlaybackSettings, OutSequenceActor);
	CurrentSequencePlayer->OnPlay.AddUniqueDynamic(this, &USequencesHandlerSubsystem::OnSequenceStarted);
	CurrentSequencePlayer->OnStop.AddUniqueDynamic(this, &USequencesHandlerSubsystem::OnSequenceStopped);
	CurrentSequencePlayer->Play();
	return true;
}

void USequencesHandlerSubsystem::Tick(float DeltaTime)
{
	if (!GetWorld() || !IsValid(CurrentSequencePlayer)) return;

	if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->WasInputKeyJustReleased(KeyboardSkipKey) || PC->WasInputKeyJustReleased(GamepadSkipKey))
		{
			SkipKeyStartTime = 0.f;
			if (IsValid(SkipSequenceWidgetInstance))
			{
				SkipSequenceWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
				//GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, TEXT("Skip Sequence Cancelled") );
			}
		}

		if (PC->WasInputKeyJustPressed(KeyboardSkipKey) || PC->WasInputKeyJustPressed(GamepadSkipKey))
		{
			if (IsValid(SkipSequenceWidgetInstance))
			{
				SkipSequenceWidgetInstance->SetVisibility(ESlateVisibility::Visible);
				//GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Green, TEXT("Skip Sequence Started") );
			}
		}
		
		if (PC->GetInputKeyTimeDown(KeyboardSkipKey) >= SkipKeyStartTime + SkipKeyHoldTime ||
			PC->GetInputKeyTimeDown(GamepadSkipKey) >= SkipKeyStartTime + SkipKeyHoldTime)
		{
			CurrentSequencePlayer->Stop();
			//GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, TEXT("Sequence Skipped") );
			
		}

		// GEngine->AddOnScreenDebugMessage( -1, DeltaTime, FColor::Yellow,
		// 	FString::Printf(TEXT("Hold '%s' to skip sequence: %.2f / %.2f"),
		// 		*KeyboardSkipKey.GetDisplayName().ToString(),
		// 		PC->GetInputKeyTimeDown(KeyboardSkipKey),
		// 		SkipKeyStartTime + SkipKeyHoldTime));
	}
}

void USequencesHandlerSubsystem::OnSequenceStarted()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Sequence Started") );

	if (!GetWorld()) return;

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		SkipKeyStartTime = FMath::Max(PC->GetInputKeyTimeDown(KeyboardSkipKey), PC->GetInputKeyTimeDown(GamepadSkipKey));

		if (SkipSequenceWidgetClass)
		{
			if (UUserWidget* SkipWidget = CreateWidget<UUserWidget, APlayerController*>(PC, SkipSequenceWidgetClass, TEXT("SkipSequenceWidget")))
			{
				SkipWidget->AddToViewport();
				SkipWidget->SetVisibility(ESlateVisibility::Collapsed);
				SkipSequenceWidgetInstance = SkipWidget;
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Skip Sequence Widget Added") );
			}
		}
	}
	
	bTickEnabled = true;
}

void USequencesHandlerSubsystem::OnSequenceStopped()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Sequence Stopped") );

	bTickEnabled = false;
	CurrentSequencePlayer = nullptr;
	if (IsValid(SkipSequenceWidgetInstance))
	{
		SkipSequenceWidgetInstance->RemoveFromParent();
		SkipSequenceWidgetInstance = nullptr;
	}
}
