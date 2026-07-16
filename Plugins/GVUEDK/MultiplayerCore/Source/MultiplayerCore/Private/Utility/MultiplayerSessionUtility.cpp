// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/MultiplayerSessionUtility.h"

UMultiplayerSessionSubsystem* UMultiplayerSessionUtility::MultiplayerSessionSubsystem = nullptr;
bool UMultiplayerSessionUtility::bInitialized = false;

void UMultiplayerSessionUtility::Init(UMultiplayerSessionSubsystem* SessionSubsystem)
{
	if (IsValid(SessionSubsystem))
	{
		MultiplayerSessionSubsystem = SessionSubsystem;
		bInitialized = true;
	}
}

void UMultiplayerSessionUtility::CreateSession(const FMultiplayerSessionSettings& Settings)
{
	if (!bInitialized) return;
	MultiplayerSessionSubsystem->CreateSession(Settings);
}

void UMultiplayerSessionUtility::FindSessions(const FMultiplayerFindSettings& Settings)
{
	if (!bInitialized) return;
	MultiplayerSessionSubsystem->FindSessions(Settings);
}

void UMultiplayerSessionUtility::FindFriendSession(const FUniqueNetIdRepl& Friend)
{
	if (!bInitialized) return;
	MultiplayerSessionSubsystem->FindFriendSession(*Friend.GetUniqueNetId());
}

void UMultiplayerSessionUtility::JoinSession(const FBlueprintSessionResult& SessionToJoin)
{
	if (!bInitialized) return;
	MultiplayerSessionSubsystem->JoinSession(SessionToJoin.OnlineResult);
}

void UMultiplayerSessionUtility::DestroySession()
{
	if (!bInitialized) return;
	MultiplayerSessionSubsystem->DestroySession();
}

void UMultiplayerSessionUtility::EndSession()
{
	if (!bInitialized) return;
	MultiplayerSessionSubsystem->EndSession();
}

void UMultiplayerSessionUtility::UpdateSessionSearchKeyword(const FString& NewSearchKeyword)
{
	TMap<FName, bool> EmptyTemp;
	UpdateSession(NewSearchKeyword, EmptyTemp);
}

void UMultiplayerSessionUtility::UpdateSessionFloat(const TMap<FName, float>& NewSessionData)
{
	UpdateSession(FString(), NewSessionData);
}

void UMultiplayerSessionUtility::UpdateSessionString(const TMap<FName, FString>& NewSessionData)
{
	UpdateSession(FString(), NewSessionData);
}

void UMultiplayerSessionUtility::UpdateSessionBool(const TMap<FName, bool>& NewSessionData)
{
	UpdateSession(FString(), NewSessionData);
}

FString UMultiplayerSessionUtility::GetLastSessionSearchKeyword()
{
	if (!bInitialized) return FString();
	return MultiplayerSessionSubsystem->GetLastSessionSearchKeyword();
}