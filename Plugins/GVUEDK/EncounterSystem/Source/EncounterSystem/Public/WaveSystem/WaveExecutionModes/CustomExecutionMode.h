// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/WaveExecutionMode.h"
#include "CustomExecutionMode.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ENCOUNTERSYSTEM_API UCustomExecutionMode : public UWaveExecutionMode
{
	GENERATED_BODY()

public:

	virtual bool Init(UWaveExecutionMode* Template, UWaveManager* InWaveManager, UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex) override;

	virtual void ExecuteWave() override;

	virtual UWaveData* GetNextWaveData() override;

	virtual int32 GetNextWaveIndex() override;

	virtual bool HasNextWave() override;

	/** Blueprint event to initialize the custom execution mode with specific parameters.
	 * @param Template - The template execution mode to copy settings from (cast to appropriate type in Blueprint).
	 * @param InWaveManager - The wave manager that owns this execution mode.
	 * @param InOwningWaveData - The wave data that this execution mode is associated with.
	 * @param InCurrentWaveIndex - The index of the current wave.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void Init(const UWaveExecutionMode* Template, const UWaveManager* InWaveManager, const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex);

	/** Blueprint event to execute the wave with custom logic.
	 * Call the parent function to ensure the OnWaveStarted event is broadcasted (needed to trigger other systems i.e. spawners).
	 * @param InWaveManager - The wave manager that owns this execution mode.
	 * @param InOwningWaveData - The wave data that this execution mode is associated with.
	 * @param InCurrentWaveIndex - The index of the current wave.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void ExecuteWave(const UWaveManager* InWaveManager, const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex);
	void ExecuteWave_Implementation(const UWaveManager* InWaveManager, const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex);

	/** Blueprint event to get the next wave data based on custom logic.
	 * Usually returns the same wave data, but can return different data for branching waves.
	 * @param InWaveManager - The wave manager that owns this execution mode.
	 * @param InOwningWaveData - The wave data that this execution mode is associated with.
	 * @param InCurrentWaveIndex - The index of the current wave.
	 * @return The next wave data to be executed.
	 */
	UFUNCTION(BlueprintNativeEvent)
	UWaveData* GetNextWaveData(const UWaveManager* InWaveManager, const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex);
	UWaveData* GetNextWaveData_Implementation(const UWaveManager* InWaveManager, const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex);

	/** Blueprint event to get the next wave index based on custom logic.
	 * By default, this would be CurrentWaveIndex + 1, but can be customized for branching waves.
	 * @param InWaveManager - The wave manager that owns this execution mode.
	 * @param InOwningWaveData - The wave data that this execution mode is associated with.
	 * @param InCurrentWaveIndex - The index of the current wave.
	 * @return The index of the next wave to be executed.
	 */
	UFUNCTION(BlueprintNativeEvent)
	int32 GetNextWaveIndex(const UWaveManager* InWaveManager, const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex);
	int32 GetNextWaveIndex_Implementation(const UWaveManager* InWaveManager, const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex);

	/** Blueprint event to determine if there is a next wave based on custom logic.
	 * By default, check the existence of the next wave index in the owning wave data.
	 * @param InWaveManager - The wave manager that owns this execution mode.
	 * @param InOwningWaveData - The wave data that this execution mode is associated with.
	 * @param InCurrentWaveIndex - The index of the current wave.
	 * @return True if there is a next wave, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool HasNextWave(const UWaveManager* InWaveManager, const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex);
	bool HasNextWave_Implementation(const UWaveManager* InWaveManager, const UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex);

private:
	
	UPROPERTY()
	UWorld* World = nullptr;

#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override { return true; }
# endif

	virtual UWorld* GetWorld() const override { return World; }
	
};
