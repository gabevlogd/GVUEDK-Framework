// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/WaveData.h"
#include "EnemyTracker/EnemyTracker.h"
#include "Subsystems/WorldSubsystem.h"
#include "WaveManager.generated.h"

class UWaveCompletionModeFactory;
class UWaveExecutionModeFactory;
class UWaveEventsHandler;
DEFINE_LOG_CATEGORY_STATIC(LogWaveManagerSubsystem, All, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveStarted, const UWaveData*, WaveData, const int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFirstWaveStarted, const UWaveData*, WaveData, const int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveCompleted, const UWaveData*, WaveData, const int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveCanceled, const UWaveData*, WaveData, const int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAllWavesCompleted, const UWaveData*, WaveData, const int32, LastWaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllEnemiesSpawned);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDead, AActor*, DeadEnemy);

UCLASS(BlueprintType)
class ENCOUNTERSYSTEM_API UWaveManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category="WaveManager")
	FOnFirstWaveStarted OnFirstWaveStarted;
	
	UPROPERTY(BlueprintAssignable, Category="WaveManager")
	FOnWaveStarted OnWaveStarted; 

	UPROPERTY(BlueprintAssignable, Category="WaveManager")
	FOnWaveCompleted OnWaveCompleted;

	UPROPERTY(BlueprintAssignable, Category="WaveManager")
	FOnWaveCanceled OnWaveCanceled;

	UPROPERTY(BlueprintAssignable, Category="WaveManager")
	FOnAllWavesCompleted OnAllWavesCompleted;

	UPROPERTY(BlueprintAssignable, Category="WaveManager")
	FOnAllEnemiesSpawned OnAllEnemiesSpawned;

	UPROPERTY(BlueprintAssignable, Category="WaveManager")
	FOnEnemyDead OnEnemyDead;

private:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:

	void StartWave(UWaveData* WaveData, const int32 WaveIndex = 0);

	void StartNextWave();

	void StopCurrentWave();

	void CheckWaveCompletion();

	int32 GetGlobalAliveEnemiesCount() const;

	int32 GetCurrentWaveAliveEnemiesCount() const;

	float GetWaveRemainingTime() const;

	float GetAllWavesRemainingTime() const;

	UWaveData* GetCurrentWaveData() const;

	int32 GetCurrentWaveIndex() const;

	bool HasPendingAsyncSpawns() const { return bHasPendingAsyncSpawns; }

	FGuid GetCurrentWaveID() const { return CurrentWaveID; }

	void WaveStarted();

	bool IsAnyWaveActive() const { return bAnyActiveWave; }
	
	void CallCompletionCheckOnEnemyDeath(const bool ListenToEveryTracker = true);

private:

	void CleanUpWaveManagerState();

	void StopCallCompletionCheckOnEnemyDeath() const;

	void StopListeningSpawnManager();

	UEnemyTracker* CreateNewEnemyTracker();
	
	void WaveCompleted();

	void WaveCanceled();

	void AllWavesCompleted();

	//UFUNCTION()
	//void RemoveEnemyTracker(const FGuid& WaveID);

	UFUNCTION()
	void CheckWaveCompletion(AActor* LastDeadEnemy);
	
	void AutoStartNextWave();

	UFUNCTION()
	void AllEnemiesSpawned();

	UFUNCTION()
	void EnemyDead(AActor* DeadEnemy);

	void ClearEnemyTrackers();

	bool IsFirstWave() const;

	UPROPERTY()
	UWaveExecutionMode* CurrentWaveExecutionMode;

	UPROPERTY()
	UWaveCompletionMode* CurrentWaveCompletionMode;

	UPROPERTY()
	UWaveEventsHandler* CurrentWaveEventsHandler;

	UPROPERTY()
	FGuid CurrentWaveID;

	UPROPERTY()
	float StartTime;

	UPROPERTY()
	TMap<FGuid, UEnemyTracker*> EnemyTrackers;
	
	UPROPERTY()
	UWaveExecutionModeFactory* WaveExecutionModeFactory = nullptr;
	
	UPROPERTY()
	UWaveCompletionModeFactory* WaveCompletionModeFactory = nullptr;

	FTimerHandle NextWaveTimerHandle;

	bool bHasPendingAsyncSpawns = false;

	bool bAnyActiveWave = false;
};
