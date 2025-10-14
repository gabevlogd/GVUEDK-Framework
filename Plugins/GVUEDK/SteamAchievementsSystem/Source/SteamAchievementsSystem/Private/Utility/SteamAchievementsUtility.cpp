// Copyright The Prototypers, Inc. All Rights Reserved.


#include "Utility/SteamAchievementsUtility.h"
#include "Subsystems/SteamAchievementsSubsystem.h"

USteamAchievementsSubsystem* USteamAchievementsUtility::SteamAchievementsSubsystem = nullptr;

void USteamAchievementsUtility::Initialize(USteamAchievementsSubsystem* Subsystem)
{
	if (Subsystem == nullptr)
	{
		UE_LOG(LogSteamAchievements, Error, TEXT("SteamAchievementsUtility: Initialize called with null Subsystem"));
		return;
	}
	
	SteamAchievementsSubsystem = Subsystem;
}

bool USteamAchievementsUtility::UnlockAchievementRequest(const FName AchName, const float Progress)
{
	if (!Initialized()) return false;
	return SteamAchievementsSubsystem->UnlockAchievementRequest(AchName, Progress);
}

void USteamAchievementsUtility::SetMetaAchievementName(const FName& InMetaName)
{
	if (!Initialized()) return;
	SteamAchievementsSubsystem->MetaAchievementId = InMetaName;
}
