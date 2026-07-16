// Fill out your copyright notice in the Description page of Project Settings.


#include "Sessions/MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Utility/MultiplayerSessionUtility.h"

IOnlineSessionPtr UMultiplayerSessionSubsystem::GetSessionInterface() const
{
	if (!IsValid(GetWorld())) return nullptr;
	
	if (const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
	{
		return Subsystem->GetSessionInterface();
	}
	return nullptr;
}

void UMultiplayerSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Initialize the utility class for blueprint access
	UMultiplayerSessionUtility::Init(this);

	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogMultiplayerSession, Error, TEXT("MultiplayerSessionSubsystem: No valid session interface found during initialization"));
		return;
	}

	SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(
		FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::HandleSessionUserInviteAccepted));
}

void UMultiplayerSessionSubsystem::CreateSession(const FMultiplayerSessionSettings& Settings)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		OnCreateSessionComplete.Broadcast(false);
		return;
	}

	if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
	{
		OnCreateSessionComplete.Broadcast(false);
		return;
	}

	// Destroy existing session if any, to avoid conflicts
	SessionInterface->DestroySession(NAME_GameSession);
	
	LastSessionSettings = MakeShared<FOnlineSessionSettings>();
	LastSessionSettings->NumPublicConnections             = Settings.NumPublicConnections;
	LastSessionSettings->NumPrivateConnections            = Settings.NumPrivateConnections;
	LastSessionSettings->bIsLANMatch                      = Settings.bIsLAN;
	LastSessionSettings->bShouldAdvertise                 = Settings.bShouldAdvertise;
	LastSessionSettings->bAllowJoinInProgress             = Settings.bAllowJoinInProgress;
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = Settings.bAllowJoinViaPresenceFriendsOnly;
	LastSessionSettings->bAllowInvites 					  = Settings.bAllowInvites;
	

	// Generic flags
	LastSessionSettings->bUsesPresence = Settings.bUsesPresence;

	// Important for Steam: must match host/client usage
	LastSessionSettings->bUseLobbiesIfAvailable = Settings.bUseLobbiesIfAvailable;

	// If presence enabled, allow join via presence/friends
	LastSessionSettings->bAllowJoinViaPresence = Settings.bUsesPresence;

	LastSessionSettings->BuildUniqueId = GetBuildUniqueId();

	// Publish a tag to filter search results (critical with SteamDevAppId=480)
	if (!Settings.SearchKeyword.IsEmpty())
	{
		LastSessionSettings->Set(SEARCH_KEYWORDS, Settings.SearchKeyword, EOnlineDataAdvertisementType::ViaOnlineService);
	}

	// Bind delegate
	CreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::HandleCreateSessionComplete));

	// Local player index 0
	const ULocalPlayer* LP = GetGameInstance() ? GetGameInstance()->GetFirstGamePlayer() : nullptr;
	const FUniqueNetIdRepl UserId = LP ? LP->GetPreferredUniqueNetId() : FUniqueNetIdRepl();

	if (!UserId.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
		OnCreateSessionComplete.Broadcast(false);
		return;
	}
	
	if (!SessionInterface->CreateSession(*UserId, NAME_GameSession, *LastSessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
		OnCreateSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionSubsystem::HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);

		if (bWasSuccessful)
		{
			SessionInterface->StartSession(SessionName);
		}
	}

	OnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionSubsystem::FindSessions(const FMultiplayerFindSettings& Settings)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		TArray<FBlueprintSessionResult> FoundSessions;
		OnFindSessionsComplete.Broadcast(false, 0, FoundSessions);
		return;
	}

	// Destroy existing session if any, to avoid conflicts
	SessionInterface->DestroySession(NAME_GameSession);

	LastSessionSearch = MakeShared<FOnlineSessionSearch>();
	LastSessionSearch->MaxSearchResults = Settings.MaxResults;
	LastSessionSearch->bIsLanQuery = Settings.bIsLAN;
	CachedFindSettings = Settings;

	if (Settings.bUseLobbiesIfAvailable)
	{
		LastSessionSearch->QuerySettings.Set(FName(TEXT("LOBBYSEARCH")),  true, EOnlineComparisonOp::Equals);
		LastSessionSearch->QuerySettings.Set(FName(TEXT("PRESENCESEARCH")),  true, EOnlineComparisonOp::Equals);
	}
	
	FindSessionsCompleteHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::HandleFindSessionsComplete));

	const ULocalPlayer* LP = GetGameInstance() ? GetGameInstance()->GetFirstGamePlayer() : nullptr;
	const FUniqueNetIdRepl UserId = LP ? LP->GetPreferredUniqueNetId() : FUniqueNetIdRepl();

	if (!UserId.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
		TArray<FBlueprintSessionResult> FoundSessions;
		OnFindSessionsComplete.Broadcast(false, 0, FoundSessions);
		return;
	}

	if (!SessionInterface->FindSessions(*UserId, LastSessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
		TArray<FBlueprintSessionResult> FoundSessions;
		OnFindSessionsComplete.Broadcast(false, 0, FoundSessions);
	}
}

void UMultiplayerSessionSubsystem::HandleFindSessionsComplete(bool bWasSuccessful)
{
	if (IOnlineSessionPtr SessionInterface = GetSessionInterface())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
	}

	TArray<FBlueprintSessionResult> FoundSessions = TArray<FBlueprintSessionResult>();

	if (!bWasSuccessful || !LastSessionSearch.IsValid())
	{
		OnFindSessionsComplete.Broadcast(false, 0, FoundSessions);
		return;
	}

	const FString WantedKeyword = CachedFindSettings.SearchKeyword;

	// If no keyword specified, return all results except those with keywords
	if (WantedKeyword.IsEmpty())
	{
		for (FOnlineSessionSearchResult& R : LastSessionSearch->SearchResults)
		{
			FString SearchKeyword;
			if (R.Session.SessionSettings.Get(SEARCH_KEYWORDS, SearchKeyword))
			{
				// Skip results with a keyword
				continue;
			}
			
			// Steam requires both to be true to allow joining via presence
			// in 5.5 for some reason these can be out of sync
			// so we force both to true here (Workaround)
			if (R.Session.SessionSettings.bUseLobbiesIfAvailable != R.Session.SessionSettings.bUsesPresence)
			{
				R.Session.SessionSettings.bUseLobbiesIfAvailable = true;
				R.Session.SessionSettings.bUsesPresence = true;
			}
			
			FBlueprintSessionResult NewResult;
			NewResult.OnlineResult = R;
			FoundSessions.Add(NewResult);
		}
		
		OnFindSessionsComplete.Broadcast(true, FoundSessions.Num(), FoundSessions);
		return;
	}

	// Filter results by keyword
	int32 Matches = 0;
	for (FOnlineSessionSearchResult& R : LastSessionSearch->SearchResults)
	{
		FString SearchKeyword;
		if (R.Session.SessionSettings.Get(SEARCH_KEYWORDS, SearchKeyword) && SearchKeyword == WantedKeyword)
		{
			++Matches;

			// Steam requires both to be true to allow joining via presence
			// in 5.5 for some reason these can be out of sync
			// so we force both to true here (Workaround)
			if (R.Session.SessionSettings.bUseLobbiesIfAvailable != R.Session.SessionSettings.bUsesPresence)
			{
				R.Session.SessionSettings.bUseLobbiesIfAvailable = true;
				R.Session.SessionSettings.bUsesPresence = true;
			}
			
			FBlueprintSessionResult NewResult;
			NewResult.OnlineResult = R;
			FoundSessions.Add(NewResult);
		}
	}
	
	OnFindSessionsComplete.Broadcast(true, Matches, FoundSessions);
}

void UMultiplayerSessionSubsystem::FindFriendSession(const FUniqueNetId& Friend)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		TArray<FBlueprintSessionResult> FoundSessions;
		OnFindFriendSessionComplete.Broadcast(false, 0, FoundSessions);
		return;
	}

	// Destroy existing session if any, to avoid conflicts
	SessionInterface->DestroySession(NAME_GameSession);

	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	const int32 LocalUserNum = LocalPlayer ? LocalPlayer->GetControllerId() : 0;
	TSharedPtr<const FUniqueNetId> LocalUserId = LocalPlayer ? LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId() : nullptr;

	if (!LocalUserId.IsValid())
	{
		TArray<FBlueprintSessionResult> FoundSessions;
		OnFindFriendSessionComplete.Broadcast(false, 0, FoundSessions);
		return;
	}
	
	FindFriendSessionCompleteHandle = SessionInterface->AddOnFindFriendSessionCompleteDelegate_Handle(LocalUserNum,
		FOnFindFriendSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::HandleFindFriendSessionComplete));

	if (!SessionInterface->FindFriendSession(*LocalUserId, Friend))
	{
		SessionInterface->ClearOnFindFriendSessionCompleteDelegate_Handle(LocalUserNum,FindFriendSessionCompleteHandle);
		TArray<FBlueprintSessionResult> FoundSessions;
		OnFindFriendSessionComplete.Broadcast(false, 0, FoundSessions);
	}
}

void UMultiplayerSessionSubsystem::HandleFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SearchResults)
{
	if (IOnlineSessionPtr SessionInterface = GetSessionInterface())
	{
		SessionInterface->ClearOnFindFriendSessionCompleteDelegate_Handle(LocalUserNum,FindFriendSessionCompleteHandle);
	}

	TArray<FBlueprintSessionResult> FoundSessions = TArray<FBlueprintSessionResult>();

	if (!bWasSuccessful)
	{
		OnFindFriendSessionComplete.Broadcast(false, 0, FoundSessions);
		return;
	}

	TArray<FOnlineSessionSearchResult> Mutable = SearchResults;

	for (FOnlineSessionSearchResult& R : Mutable)
	{
		// Steam requires both to be true to allow joining via presence
		// in 5.5 for some reason these can be out of sync
		// so we force both to true here (Workaround)
		if (R.Session.SessionSettings.bUseLobbiesIfAvailable != R.Session.SessionSettings.bUsesPresence)
		{
			R.Session.SessionSettings.bUseLobbiesIfAvailable = true;
			R.Session.SessionSettings.bUsesPresence = true;
		}
		
		FBlueprintSessionResult NewResult;
		NewResult.OnlineResult = R;
		FoundSessions.Add(NewResult);
	}
	
	OnFindFriendSessionComplete.Broadcast(true, FoundSessions.Num(), FoundSessions);
}

void UMultiplayerSessionSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionToJoin)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		OnJoinSessionComplete.Broadcast(false, EMultiplayerJoinResult::UnknownError);
		return;
	}

	if (!SessionToJoin.IsValid())
	{
		OnJoinSessionComplete.Broadcast(false, EMultiplayerJoinResult::SessionDoesNotExist);
		return;
	}

	if (!SessionToJoin.IsSessionInfoValid())
	{
		OnJoinSessionComplete.Broadcast(false, EMultiplayerJoinResult::UnknownError);
		return;
	}

	JoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::HandleJoinSessionComplete));

	const ULocalPlayer* LP = GetGameInstance() ? GetGameInstance()->GetFirstGamePlayer() : nullptr;
	const FUniqueNetIdRepl UserId = LP ? LP->GetPreferredUniqueNetId() : FUniqueNetIdRepl();

	if (!SessionInterface->JoinSession(*UserId, NAME_GameSession, SessionToJoin))
	{
		UE_LOG(LogMultiplayerSession, Error, TEXT("JoinSession: Failed to initiate join session"));
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
		OnJoinSessionComplete.Broadcast(false, EMultiplayerJoinResult::UnknownError);
	}
}

void UMultiplayerSessionSubsystem::HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (IOnlineSessionPtr SessionInterface = GetSessionInterface())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
	}

	EMultiplayerJoinResult BPResult;
	const bool bSuccess = (Result == EOnJoinSessionCompleteResult::Success);

	switch (Result)
	{
	case EOnJoinSessionCompleteResult::Success:
		BPResult = EMultiplayerJoinResult::Success;
		break;
	case EOnJoinSessionCompleteResult::SessionIsFull:
		BPResult = EMultiplayerJoinResult::SessionIsFull;
		break;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		BPResult = EMultiplayerJoinResult::SessionDoesNotExist;
		break;
	default:
		BPResult = EMultiplayerJoinResult::UnknownError;
		break;
	}

	if (!bSuccess)
	{
		OnJoinSessionComplete.Broadcast(false, BPResult);
		return;
	}

	// Resolve address & travel
	FString Address;
	if (const IOnlineSessionPtr SessionInterface = GetSessionInterface())
	{
		if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
		{
			OnJoinSessionComplete.Broadcast(false, EMultiplayerJoinResult::CouldNotRetrieveAddress);
			return;
		}
	}

	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			PC->ClientTravel(Address, TRAVEL_Absolute);
			OnJoinSessionComplete.Broadcast(true, EMultiplayerJoinResult::Success);
			return;
		}
	}
	
	OnJoinSessionComplete.Broadcast(false, EMultiplayerJoinResult::UnknownError);
}

void UMultiplayerSessionSubsystem::DestroySession()
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		OnDestroySessionComplete.Broadcast(false);
		return;
	}

	DestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
		FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::HandleDestroySessionComplete));

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
		OnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionSubsystem::HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
	}

	OnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionSubsystem::EndSession()
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		OnEndSessionComplete.Broadcast(false);
		return;
	}

	EndSessionCompleteHandle = SessionInterface->AddOnEndSessionCompleteDelegate_Handle(
		FOnEndSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::HandleEndSessionComplete));

	if (!SessionInterface->EndSession(NAME_GameSession))
	{
		SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(EndSessionCompleteHandle);
		OnEndSessionComplete.Broadcast(false);
	}
}

FString UMultiplayerSessionSubsystem::GetLastSessionSearchKeyword() const
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid()) return FString();
	
	FOnlineSessionSettings* SessionSettings = SessionInterface->GetSessionSettings(NAME_GameSession);
	if (!SessionSettings) return FString();
	
	FString SearchKeyword;
	if (SessionSettings->Get(SEARCH_KEYWORDS, SearchKeyword))
	{
		return SearchKeyword;
	}
	
	return FString();
}

void UMultiplayerSessionSubsystem::HandleEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(EndSessionCompleteHandle);
	}

	OnEndSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionSubsystem::HandleUpdateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(UpdateSessionCompleteHandle);
	}

	OnUpdateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionSubsystem::HandleSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum,
	FUniqueNetIdPtr LocalUserId, const FOnlineSessionSearchResult& SearchResult)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	if (!bWasSuccessful) return;

	// Steam requires both to be true to allow joining via presence
	// in 5.5 for some reason these can be out of sync
	// so we force both to true here (Workaround)
	FOnlineSessionSearchResult Mutable = SearchResult;
	Mutable.Session.SessionSettings.bUsesPresence = true;
	Mutable.Session.SessionSettings.bUseLobbiesIfAvailable = true;

	FBlueprintSessionResult InviteResult;
	InviteResult.OnlineResult = Mutable;
	OnSessionUserInviteAccepted.Broadcast(InviteResult);
}
