// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerSessionSettings.generated.h"

UENUM(BlueprintType)
enum class EMultiplayerJoinResult : uint8
{
	Success,
	SessionIsFull,
	SessionDoesNotExist,
	CouldNotRetrieveAddress,
	UnknownError
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMultiplayerSessionSettings 
{
	GENERATED_BODY()

	/**
	 * Number of public connections for the session
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumPublicConnections = 4;

	/**
	 * Number of private connections for the session
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumPrivateConnections = 0;

	/**
	 * If true, the session will be a LAN match
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLAN = false;

	/**
	 * If true, the session will use presence information
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUsesPresence = true;

	/**
	 * If true, the session will use lobbies if available (Steam)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseLobbiesIfAvailable = true;

	/**
	 * If true, players can join the session in progress
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowJoinInProgress = true;

	/**
	 * If true, the session will be advertised via online services
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldAdvertise = true;

	/**
	 * If true, only friends can join the session via presence
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowJoinViaPresenceFriendsOnly = false;


	/**
	 * If true, allows joining the session via presence (if supported by the platform)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowInvites = true;
	
	/**
	 *  Optional: keyword to filter FindSessions
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SearchKeyword;
};

USTRUCT(BlueprintType)
struct FMultiplayerFindSettings
{
	GENERATED_BODY()

	/**
	 * Maximum number of search results to return
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxResults = 2000; // if using 480 (Steam Spacewar), must be high to get all results

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLAN = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUsesPresence = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseLobbiesIfAvailable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SearchKeyword;
};
