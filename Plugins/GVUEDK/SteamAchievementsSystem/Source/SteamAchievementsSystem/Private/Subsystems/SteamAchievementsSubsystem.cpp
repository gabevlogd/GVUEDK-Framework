// // Copyright The Prototypers, Inc. All Rights Reserved.

#include "Subsystems/SteamAchievementsSubsystem.h"
#include "Utility/SteamAchievementsUtility.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemUtils.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "HAL/PlatformFilemanager.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////////////////////////////////////////////

void USteamAchievementsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	USteamAchievementsUtility::Initialize(this);
	
	ReadAchievements();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Achievement Queue
////////////////////////////////////////////////////////////////////////////////////////////////////

bool USteamAchievementsSubsystem::UnlockAchievementRequest(FName AchName, float Progress)
{
	if (AchName.IsNone() || AchName.IsEqual(""))
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("UnlockAchievementRequest: Invalid achievement name"));
		return false;
	}
	
	const FAchievementRequest Request = FAchievementRequest(AchName, Progress);

	// Avoid duplicates in the queue
	if (AchievementsQueue.Contains(Request))
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("UnlockAchievementRequest: Achievement %s already in queue"), *AchName.ToString());
		return false;
	}

	AchievementsQueue.Add(Request);

	// If not already processing the queue, start processing
	if (!bIsProcessingQueue)
	{
		UpdateAchievementsQueue();
	}

	return true;
}

void USteamAchievementsSubsystem::UpdateAchievementsQueue()
{
	bIsProcessingQueue = true;

	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("UpdateAchievementsQueue: No valid World context."));
		bIsProcessingQueue = false;
		return;
	}

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineAchievementsPtr Achievements = Subsystem ? Subsystem->GetAchievementsInterface() : nullptr;
	const IOnlineIdentityPtr Identity = Subsystem ? Subsystem->GetIdentityInterface() : nullptr;
	const TSharedPtr<const FUniqueNetId> UserId = Identity ? Identity->GetUniquePlayerId(0) : nullptr;

	// If Steam is offline, save all queued achievements locally
	if (!Subsystem || !Subsystem->IsEnabled() || !Achievements.IsValid() || !Identity.IsValid() || !UserId.IsValid() || !bIsSteamOnline)
	{
		if (AchievementsQueue.Num() > 0)
		{
			for (const FAchievementRequest& Req : AchievementsQueue)
			{
				UE_LOG(LogSteamAchievements, Warning, TEXT("Steam offline, saving achievement %s locally"), *Req.AchievementName.ToString());
				SaveLocalAchievement(Req.AchievementName, Req.AchievementProgress);
			}
			AchievementsQueue.Empty();
		}
		bIsProcessingQueue = false;
		return;
	}

	// Steam is online, process the queue

	// If the queue is empty, nothing to process
	if (AchievementsQueue.Num() == 0)
	{
		bIsProcessingQueue = false;
		return;
	}

	const FAchievementRequest Request = AchievementsQueue[0];
	AchievementsQueue.RemoveAt(0);

	FOnlineAchievementsWriteRef AchievementWrite = MakeShared<FOnlineAchievementsWrite>();
	AchievementWrite->SetFloatStat(Request.AchievementName.ToString(), Request.AchievementProgress);

	LastAchievementRequested = Request.AchievementName;

	Achievements->WriteAchievements(
		*UserId,
		AchievementWrite,
		FOnAchievementsWrittenDelegate::CreateUObject(
			this,
			&USteamAchievementsSubsystem::OnAchievementWritten 
		)
	);

}

void USteamAchievementsSubsystem::OnAchievementWritten(const FUniqueNetId& UserId, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogSteamAchievements, Log, TEXT("Achievement written successfully for user %s"), *UserId.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Achievement Unlocked: %s"), *LastAchievementRequested.ToString()));

		// If there are more achievements in the queue, continue processing
		if (AchievementsQueue.Num() > 0)
		{
			UpdateAchievementsQueue();
			return;
		}
	
		bIsProcessingQueue = false;
	
		// If the queue is empty, check if all achievements are unlocked to unlock the meta achievement
		AllAchievementUnlockedCheck();
	}
	else
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("Failed to write achievement for user %s"), *UserId.ToString());
	}
}

void USteamAchievementsSubsystem::ReadAchievements()
{
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("ReadAchievements: No valid World context."));
		return;
	}
	
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem)
	{
		UE_LOG(LogSteamAchievements, Error, TEXT("Steam Subsystem not found"));
		return;
	}

	const IOnlineAchievementsPtr Achievements = Subsystem->GetAchievementsInterface();
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	if (!Achievements.IsValid() || !Identity.IsValid())
	{
		UE_LOG(LogSteamAchievements, Error, TEXT("Achievements or Identity interface invalid"));
		return;
	}

	const FUniqueNetIdPtr UserId = Identity->GetUniquePlayerId(0);
	if (!UserId.IsValid())
	{
		UE_LOG(LogSteamAchievements, Error, TEXT("User ID invalid, cannot read achievements"));
		return;
	}

	Achievements->QueryAchievements(
		*UserId,
		FOnQueryAchievementsCompleteDelegate::CreateUObject(
			this,
			&USteamAchievementsSubsystem::OnAchievementsRead
		)
	);
}

void USteamAchievementsSubsystem::OnAchievementsRead(const FUniqueNetId& UserId, bool bWasSuccessful) 
{
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("OnAchievementsRead: No valid World context."));
		return;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem)
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("OnAchievementsRead: No OnlineSubsystem found."));
		return;
	}
	
	IOnlineAchievementsPtr Achievements = Subsystem->GetAchievementsInterface();
	if (!Achievements.IsValid())
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("OnAchievementsRead: No AchievementsInterface found."));
		return;
	}

	// Get the cached achievements
	TArray<FOnlineAchievement> PlayerAchievements;
	Achievements->GetCachedAchievements(UserId, PlayerAchievements);

	// Cycle through achievements to check if they are valid
	for (const FOnlineAchievement& Ach : PlayerAchievements)
	{
		UE_LOG(LogSteamAchievements, Log, TEXT(" - %s : %f"), *Ach.Id, Ach.Progress);

		// If any achievement ID contains "null", Steam is likely offline or there was an error
		if (Ach.Id.Contains("null", ESearchCase::IgnoreCase, ESearchDir::FromStart))
		{
			bIsSteamOnline = false;
			UE_LOG(LogSteamAchievements, Warning, TEXT("Cant read achievements, Steam might be offline, saving locally"));
			return;
		}
	}
	
	// If we reach here, achievements were read successfully
	bIsSteamOnline = true;
	UE_LOG(LogSteamAchievements, Log, TEXT("Successfully read %d achievements for user %s"), PlayerAchievements.Num(), *UserId.ToString());

	// After successfully reading achievements, try to sync any local achievements saved while offline
	UE_LOG(LogSteamAchievements, Log, TEXT("Attempting to sync local achievements..."));
	SyncLocalAchievements();
}

void USteamAchievementsSubsystem::AllAchievementUnlockedCheck()
{
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("AllAchievementUnlockedCheck: No valid World context."));
		return;
	}
	
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem)
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("AllAchievementUnlockedCheck: No OnlineSubsystem found."));
		return;
	}

	IOnlineAchievementsPtr Achievements = Subsystem->GetAchievementsInterface();
	if (!Achievements.IsValid())
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("AllAchievementUnlockedCheck: No AchievementsInterface found."));
		return;
	}

	FUniqueNetIdPtr UserId = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0);
	if (!UserId.IsValid())
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("AllAchievementUnlockedCheck: No valid UserId."));
		return;
	}

	
	Achievements->QueryAchievements(*UserId, FOnQueryAchievementsCompleteDelegate::CreateLambda(
		[this](const FUniqueNetId& QueriedUserId, const bool bWasSuccessful)
		{
			if (!bWasSuccessful)
			{
				UE_LOG(LogSteamAchievements, Warning, TEXT("AllAchievementUnlockedCheck: Failed to query achievements."));
				return;
			}

			IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
			IOnlineAchievementsPtr Achievements = Subsystem->GetAchievementsInterface();

			// Get the cached achievements from Steam
			TArray<FOnlineAchievement> AchList;
			Achievements->GetCachedAchievements(QueriedUserId, AchList);

			bool bAllUnlocked = true;

			// Check if all achievements are unlocked
			for (const FOnlineAchievement& Ach : AchList)
			{
				// if meta achievement is locked just skip it, else if already unlocked, we are done
				if (Ach.Id == MetaAchievementId)
				{
					if (Ach.Progress == 0.f)
					{
						continue;
					}
					return;
				}
				
				if (Ach.Progress == 0.f)
				{
					bAllUnlocked = false;
					break;
				}
			}

			if (bAllUnlocked)
			{
				UE_LOG(LogSteamAchievements, Log, TEXT("All achievements unlocked! Unlocking META achievement."));
				UnlockAchievementRequest(MetaAchievementId, 1.f);
			}
			else
			{
				UE_LOG(LogSteamAchievements, Log, TEXT("Not all achievements unlocked yet."));
			}
		}
	));
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Local File Handling for Offline mode
////////////////////////////////////////////////////////////////////////////////////////////////////

FString USteamAchievementsSubsystem::GetLocalAchievementsFilePath() const
{
	return FPaths::ProjectSavedDir() / TEXT("SaveGames/achievements/LocalAchievements.json");
}

void USteamAchievementsSubsystem::SaveLocalAchievement(const FName& AchievementId, const float Progress) const
{
	TArray<FAchievementRequest> LocalAchievements;
	LoadLocalAchievements(LocalAchievements);

	// Avoid duplicates, only save the first occurrence
	for (const auto& Ach : LocalAchievements)
	{
		if (Ach.AchievementName == AchievementId)
			return;
	}

	// Add new achievement
	LocalAchievements.Add(FAchievementRequest(AchievementId, Progress));

	// Writing to JSON
	FString OutputString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);

	JsonWriter->WriteArrayStart();
	for (const auto& Ach : LocalAchievements)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("Id"), Ach.AchievementName.ToString());
		JsonWriter->WriteValue(TEXT("Progress"), Ach.AchievementProgress);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->Close();

	FFileHelper::SaveStringToFile(OutputString, *GetLocalAchievementsFilePath());

	UE_LOG(LogSteamAchievements, Log, TEXT("Saved local achievement %s"), *AchievementId.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Locally Saved Achievement: %s"), *AchievementId.ToString()));
}


void USteamAchievementsSubsystem::LoadLocalAchievements(TArray<FAchievementRequest>& OutAchievements) const
{
	// Get the file path of the local achievements JSON file
	FString FilePath = GetLocalAchievementsFilePath();
	if (!FPaths::FileExists(FilePath))
		return;

	// Read the JSON file
	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
		TArray<TSharedPtr<FJsonValue>> JsonArray;

		if (FJsonSerializer::Deserialize(Reader, JsonArray))
		{
			for (auto& Value : JsonArray)
			{
				TSharedPtr<FJsonObject> Obj = Value->AsObject();
				if (!Obj.IsValid())
					continue;

				FString Id = Obj->GetStringField(StringCast<TCHAR>("Id"));
				const float Progress = Obj->GetNumberField(StringCast<TCHAR>("Progress"));

				// Add recovered achievement to output array
				OutAchievements.Add(FAchievementRequest(FName(Id), Progress));
			}
		}
	}
}

void USteamAchievementsSubsystem::SyncLocalAchievements()
{
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("SyncLocalAchievements: No valid World context."));
		return;
	}
	
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem || !Subsystem->IsEnabled())
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("SyncLocalAchievements: No OnlineSubsystem found or not enabled."));
		return;
	}

	IOnlineAchievementsPtr Achievements = Subsystem->GetAchievementsInterface();
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	if (!Achievements.IsValid() || !Identity.IsValid())
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("SyncLocalAchievements: Achievements or Identity interface invalid."));
		return;
	}

	FUniqueNetIdPtr UserId = Identity->GetUniquePlayerId(0);
	if (!UserId.IsValid())
	{
		UE_LOG(LogSteamAchievements, Warning, TEXT("SyncLocalAchievements: No valid UserId."));
		return;
	}

	// Load local achievements from file
	TArray<FAchievementRequest> LocalAchievements;
	LoadLocalAchievements(LocalAchievements);

	// If no local achievements, nothing to sync
	if (LocalAchievements.Num() == 0)
	{
		UE_LOG(LogSteamAchievements, Log, TEXT("No local achievements to sync."));
		return;
	}

	// Sync each local achievement with Steam by adding them to the queue
	for (const FAchievementRequest& Ach : LocalAchievements)
	{
		FOnlineAchievementsWriteRef WriteObject = MakeShared<FOnlineAchievementsWrite>();
		UnlockAchievementRequest(Ach.AchievementName, Ach.AchievementProgress);
	}

	// After syncing, delete the local achievements file
	IFileManager::Get().Delete(*GetLocalAchievementsFilePath());

	UE_LOG(LogSteamAchievements, Log, TEXT("Synchronized %d local achievements with Steam"), LocalAchievements.Num());
}

