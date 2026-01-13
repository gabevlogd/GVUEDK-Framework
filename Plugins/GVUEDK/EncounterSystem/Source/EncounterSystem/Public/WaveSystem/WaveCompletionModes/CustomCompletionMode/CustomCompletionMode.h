// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CompletionCondition.h"
#include "WaveSystem/WaveCompletionModes/Base/WaveCompletionMode.h"
#include "CustomCompletionMode.generated.h"

class UWaveData;
/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UCustomCompletionMode : public UWaveCompletionMode
{
	GENERATED_BODY()

public:

	virtual bool Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager) override;
	
	virtual bool IsWaveComplete() override;


private:

	/**
	 *  The class of condition used to determine if the wave is complete (Subclass of UCompletionCondition).
	 *  NOTE: the condition check is done every time an enemy is killed by default, or when manually requested via CheckWaveCompletion() in the Encounter System Utility.
	 */
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCompletionCondition> CompletionConditionClass;

	UPROPERTY()
	UCompletionCondition* CompletionConditionInstance = nullptr;

	/**
	 *  Use this object to store any meta-data needed for the custom completion condition.
	 */
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObject> MetaClass = nullptr;

	UPROPERTY()
	UObject* Meta = nullptr;

	/**
	 * If true, the WaveManager will call CheckWaveCompletion() each time an enemy dies
	 */
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	bool bCompletionCheckOnEnemyDeath = true;
	
};
