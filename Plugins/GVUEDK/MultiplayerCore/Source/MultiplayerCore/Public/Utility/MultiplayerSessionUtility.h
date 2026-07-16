// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sessions/MultiplayerSessionSubsystem.h"
#include "MultiplayerSessionUtility.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERCORE_API UMultiplayerSessionUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static void Init(UMultiplayerSessionSubsystem* SessionSubsystem);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Core")
	static void CreateSession(const FMultiplayerSessionSettings& Settings);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Core")
	static void FindSessions(const FMultiplayerFindSettings& Settings);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Core")
	static void FindFriendSession(const FUniqueNetIdRepl& Friend);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Core")
	static void JoinSession(const FBlueprintSessionResult& SessionToJoin);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Core")
	static void DestroySession();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Core")
	static void EndSession();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Core")
	static void UpdateSessionSearchKeyword(const FString& NewSearchKeyword);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Core")
	static void UpdateSessionFloat(const TMap<FName, float>& NewSessionData);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Core")
	static void UpdateSessionString(const TMap<FName, FString>& NewSessionData);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Core")
	static void UpdateSessionBool(const TMap<FName, bool>& NewSessionData);

	template <typename ValueType>
	static void UpdateSession(const FString& NewSearchKeyword, const TMap<FName, ValueType>& NewSessionData)
	{
		if (!bInitialized) return;
		MultiplayerSessionSubsystem->UpdateSession(NewSearchKeyword, NewSessionData);
	}

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Core", BlueprintPure)
	static FString GetLastSessionSearchKeyword();

private:

	static UMultiplayerSessionSubsystem* MultiplayerSessionSubsystem;

	static bool bInitialized;

};



