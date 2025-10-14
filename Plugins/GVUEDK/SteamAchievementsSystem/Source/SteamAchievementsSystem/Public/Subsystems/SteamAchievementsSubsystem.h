// Copyright The Prototypers, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SteamAchievementsSubsystem.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogSteamAchievements, All, All);

USTRUCT()
struct FAchievementRequest
{
	GENERATED_BODY()

	UPROPERTY()
	FName AchievementName;

	UPROPERTY()
	float AchievementProgress = 1.f;

	FAchievementRequest() = default;

	FAchievementRequest(const FName AchName, const float Progress)
		: AchievementName(AchName)
		, AchievementProgress(Progress)
	{}

	bool operator==(const FAchievementRequest& Other) const
	{
		return AchievementName == Other.AchievementName && AchievementProgress == Other.AchievementProgress;
	}
};

USTRUCT()
struct FLocalAchievementData
{
	GENERATED_BODY()

	UPROPERTY()
	FString Id;

	UPROPERTY()
	float Progress;

	FLocalAchievementData() : Id(TEXT("")), Progress(0.f) {}
	FLocalAchievementData(const FString& InId, const float InProgress) : Id(InId), Progress(InProgress) {}
};

/**
 * 
 */
UCLASS()
class STEAMACHIEVEMENTSSYSTEM_API USteamAchievementsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class USteamAchievementsUtility;

private:
	
	TArray<FAchievementRequest> AchievementsQueue;

	bool bIsProcessingQueue = false;
	
	FName MetaAchievementId = TEXT("ACH_META");

	FName LastAchievementRequested;

	bool bIsSteamOnline = false;

private:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	bool UnlockAchievementRequest(const FName AchName, const float Progress = 1.f);
	
	void UpdateAchievementsQueue();
	
	void OnAchievementWritten(const FUniqueNetId& UserId, bool bWasSuccessful);

	void ReadAchievements();
	
	void OnAchievementsRead(const FUniqueNetId& UserId, bool bWasSuccessful);

	void AllAchievementUnlockedCheck();

	// Set the Meta Achievement ID, which will be unlocked when all other achievements are unlocked
	void SetMetaAchievementName(const FName& InMetaName) { MetaAchievementId = InMetaName; }


	//Local Achievement Handling (for offline mode)

	FString GetLocalAchievementsFilePath() const;
	
	void SaveLocalAchievement(const FName& AchievementId, const float Progress = 1.f) const;
	
	void LoadLocalAchievements(TArray<FAchievementRequest>& OutAchievements) const;
	
	void SyncLocalAchievements();
};
