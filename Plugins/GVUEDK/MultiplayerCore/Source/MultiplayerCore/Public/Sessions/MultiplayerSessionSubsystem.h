// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerSessionSettings.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FindSessionsCallbackProxy.h" 
#include "MultiplayerSessionSubsystem.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogMultiplayerSession, All, All);

static const FName SEARCH_KEYWORDS (TEXT("GABEVLOGD_KEY"));

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateSessionCompleteBP, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFindSessionsCompleteBP, bool, bSuccess, int32, NumResults, const TArray<FBlueprintSessionResult>&, FoundSessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFindFriendSessionCompleteBP, bool, bSuccess, int32, NumResults, const TArray<FBlueprintSessionResult>&, FoundSessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJoinSessionCompleteBP, bool, bSuccess, EMultiplayerJoinResult, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDestroySessionCompleteBP, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndSessionCompleteBP, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateSessionCompleteBP, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionUserInviteAcceptedBP, const FBlueprintSessionResult&, SearchResult);

/**
 * 
 */
UCLASS()
class MULTIPLAYERCORE_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Core")
	FOnCreateSessionCompleteBP OnCreateSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Core")
	FOnFindSessionsCompleteBP OnFindSessionsComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Core")
	FOnFindFriendSessionCompleteBP OnFindFriendSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Core")
	FOnJoinSessionCompleteBP OnJoinSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Core")
	FOnDestroySessionCompleteBP OnDestroySessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Core")
	FOnEndSessionCompleteBP OnEndSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Core")
	FOnUpdateSessionCompleteBP OnUpdateSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Core")
	FOnSessionUserInviteAcceptedBP OnSessionUserInviteAccepted;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void CreateSession(const FMultiplayerSessionSettings& Settings);

	void FindSessions(const FMultiplayerFindSettings& Settings);

	void FindFriendSession(const FUniqueNetId& Friend);

	void JoinSession(const FOnlineSessionSearchResult& SessionToJoin);

	void DestroySession();

	void EndSession();

	template <typename ValueType>
	void UpdateSession(const FString& NewSearchKeyword, const TMap<FName, ValueType>& NewSessionData);

	FString GetLastSessionSearchKeyword() const;

private:

	IOnlineSessionPtr GetSessionInterface() const;

	void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void HandleFindSessionsComplete(bool bWasSuccessful);
	void HandleFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SearchResults);
	void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void HandleEndSessionComplete(FName SessionName, bool bWasSuccessful);
	void HandleUpdateSessionComplete(FName SessionName, bool bWasSuccessful);
	void HandleSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum, FUniqueNetIdPtr LocalUserId, const FOnlineSessionSearchResult& SearchResult);
	
	FDelegateHandle CreateSessionCompleteHandle;
	FDelegateHandle FindSessionsCompleteHandle;
	FDelegateHandle FindFriendSessionCompleteHandle;
	FDelegateHandle JoinSessionCompleteHandle;
	FDelegateHandle DestroySessionCompleteHandle;
	FDelegateHandle EndSessionCompleteHandle;
	FDelegateHandle UpdateSessionCompleteHandle;
	
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	FMultiplayerFindSettings CachedFindSettings;
};

template <typename ValueType>
void UMultiplayerSessionSubsystem::UpdateSession(const FString& NewSearchKeyword, const TMap<FName, ValueType>& NewSessionData)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		OnUpdateSessionComplete.Broadcast(false);
		return;
	}

	TSharedPtr<FOnlineSessionSettings> UpdatedSessionSettings = MakeShareable(new FOnlineSessionSettings(*LastSessionSettings));

	if (!NewSearchKeyword.IsEmpty())
	{
		UpdatedSessionSettings->Set<FString>(SEARCH_KEYWORDS, NewSearchKeyword, EOnlineDataAdvertisementType::ViaOnlineService);
	}

	for (auto Element : NewSessionData)
	{
		UpdatedSessionSettings->Set<ValueType>(Element.Key, Element.Value, EOnlineDataAdvertisementType::ViaOnlineService);
	}

	UpdateSessionCompleteHandle = SessionInterface->AddOnUpdateSessionCompleteDelegate_Handle(
		FOnUpdateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::HandleUpdateSessionComplete));
	
	if (!SessionInterface->UpdateSession(NAME_GameSession, *UpdatedSessionSettings))
	{
		SessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(UpdateSessionCompleteHandle);
		OnUpdateSessionComplete.Broadcast(false);
	}
	else
	{
		LastSessionSettings = UpdatedSessionSettings;
	}
}


