// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WaveEventsHandler.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced, meta=(HideCategories="Hidden"))
class ENCOUNTERSYSTEM_API UWaveEventsHandler : public UObject
{
	GENERATED_BODY()

public:

	virtual void Init(class UWaveManager* InWaveManager, class UWaveData* InWaveData, const int32 InWaveIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnInitialize(UWaveManager* InWaveManager, UWaveData* InWaveData, const int32 InWaveIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnWaveStarted(UWaveManager* InWaveManager, UWaveData* InWaveData, const int32 InWaveIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnWaveCompleted(UWaveManager* InWaveManager, UWaveData* InWaveData, const int32 InWaveIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnWaveCanceled(UWaveManager* InWaveManager, UWaveData* InWaveData, const int32 InWaveIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAllWavesCompleted(UWaveManager* InWaveManager, UWaveData* InWaveData, const int32 InWaveIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAllEnemiesSpawned(UWaveManager* InWaveManager, UWaveData* InWaveData, const int32 InWaveIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnEnemyDead(UWaveManager* InWaveManager, UWaveData* InWaveData, const int32 InWaveIndex, AActor* DeadEnemy);
	
private:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
	UWaveManager* WaveManager;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
	UWaveData* WaveData;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
	int32 WaveIndex;

	UPROPERTY()
	UWorld* World = nullptr;

#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override { return true; }
# endif

	virtual UWorld* GetWorld() const override { return World; }
};
