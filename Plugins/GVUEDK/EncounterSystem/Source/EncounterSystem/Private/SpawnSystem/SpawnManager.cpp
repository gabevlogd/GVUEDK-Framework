// Copyright Villains, Inc. All Rights Reserved.


#include "SpawnSystem/SpawnManager.h"

#include "SpawnSystem/AsyncSpawnHandler.h"
#include "SpawnSystem/SpawnMethods/Base/SpawnMethod.h"
#include "SpawnSystem/Factories/SpawnMethodFactory.h"
#include "Utility/EncounterSystemUtility.h"

void USpawnManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	EncounterSystemSettings = GetDefault<UEncounterSystemSettings>();

	if (!IsValid(EncounterSystemSettings))
	{
		UE_LOG(LogSpawnManagerSubsystem, Error, TEXT("SpawnManagerSubsystem: EncounterSystemSettings is not valid."));
		return;
	}

	const TSubclassOf<USpawnOperation> SpawnOperationClass = EncounterSystemSettings->DefaultSpawnOperation;
	if (!IsValid(SpawnOperationClass))
	{
		UE_LOG(LogSpawnManagerSubsystem, Error, TEXT("SpawnManagerSubsystem: DefaultSpawnOperation class is not valid in EncounterSystemSettings."));
		return;
	}
	
	SpawnOperation = NewObject<USpawnOperation>(this, SpawnOperationClass);

	UEncounterSystemUtility::InitializeSpawnSystem(this);

	Collection.InitializeDependency(UWaveManager::StaticClass());

	if (UWaveManager* WaveManager = GetWorld()->GetSubsystem<UWaveManager>())
	{
		WaveManager->OnWaveStarted.AddUniqueDynamic(this, &USpawnManager::SpawnWave);
		WaveManager->OnWaveCanceled.AddUniqueDynamic(this, &USpawnManager::StopPendingSpawnRequests);
	}
	else
	{
		UE_LOG(LogSpawnManagerSubsystem, Error, TEXT("SpawnManagerSubsystem: Failed to get WaveManager subsystem."));
	}
}

void USpawnManager::Deinitialize()
{
	Super::Deinitialize();

	UEncounterSystemUtility::DeinitializeSpawnSystem();
}

AActor* USpawnManager::SpawnEnemyAtLocation(const TSubclassOf<AActor> EnemyClass, const FVector& Location, const FRotator& Rotation) const
{
	UWorld* World = GetWorld();
	
	if (!IsValid(World))
	{
		UE_LOG(LogSpawnManagerSubsystem, Error, TEXT("SpawnEnemyAtLocation called but World is null"));
		return nullptr;
	}

	if (!IsValid(EnemyClass))
	{
		UE_LOG(LogSpawnManagerSubsystem, Error, TEXT("SpawnEnemyAtLocation called with null EnemyClass"));
		return nullptr;
	}

	if (AActor* Enemy = SpawnOperation->SpawnActor(World, EnemyClass, Location, Rotation))
	{
		OnEnemySpawned.Broadcast(Enemy);
		return Enemy;
	}

	UE_LOG(LogSpawnManagerSubsystem, Warning, TEXT("Failed to spawn enemy of class %s at location %s"), *EnemyClass->GetName(), *Location.ToString());
	return nullptr;
}

void USpawnManager::SpawnGroup(const FEnemyGroup& EnemyGroup)
{
	if (!IsValid(EnemyGroup.SpawnMethod))
	{
		UE_LOG(LogSpawnManagerSubsystem, Error, TEXT("Trying to spawn EnemyGroup with null SpawnMethod"));
		return;
	}

	USpawnMethod* SpawnMethodInstance = USpawnMethodFactory::CreateSpawnMethod(EnemyGroup.SpawnMethod, this);
	if (!IsValid(SpawnMethodInstance))
	{
		UE_LOG(LogSpawnManagerSubsystem, Error, TEXT("SpawnMethodFactory failed to create a valid SpawnMethod instance"));
		return;
	}

	// If SpawnInterval or SpawnDelay is greater than 0, use async spawning
	UAsyncSpawnHandler* SpawnHandler = nullptr;
	if (!IsRangeZero(EnemyGroup.SpawnIntervalRange) || !IsRangeZero(EnemyGroup.SpawnDelayRange))
	{
		SpawnHandler = NewObject<UAsyncSpawnHandler>(this);
		SpawnHandler->Init(this, EnemyGroup.SpawnDelayRange, EnemyGroup.SpawnIntervalRange);
		SpawnHandler->AddToRoot();
		AsyncSpawnHandlerCount++;
	}

	for (const FEnemyTypeInfo& EnemyTypeInfo : EnemyGroup.EnemyTypes)
	{
		FRotator SpawnRotation = FRotator::ZeroRotator;
		FVector SpawnLocation = FVector::ZeroVector;

		const int32 EnemyCount = GetCountFromRange(EnemyTypeInfo.CountRange);
		
		for (int32 i = 0; i <  EnemyCount; ++i)
		{
			if (!SpawnMethodInstance->FindSpawnLocation(GetWorld(), nullptr, SpawnLocation))
			{
				UE_LOG(LogSpawnManagerSubsystem, Warning, TEXT("SpawnMethod failed to find a valid spawn location"));
				continue;
			}
			if (!SpawnMethodInstance->FindSpawnRotation(GetWorld(), nullptr, SpawnRotation))
			{
				UE_LOG(LogSpawnManagerSubsystem, Warning, TEXT("SpawnMethod failed to find a valid spawn rotation"));
				continue;
			}
			if (IsValid(SpawnHandler))
			{
				SpawnHandler->AddSpawnRequest(FSpawnRequest(EnemyTypeInfo.EnemyClass, SpawnLocation + EnemyTypeInfo.PivotOffset, SpawnRotation));
				continue;
			}
			const AActor* SpawnedEnemy = SpawnEnemyAtLocation(EnemyTypeInfo.EnemyClass, SpawnLocation + EnemyTypeInfo.PivotOffset, SpawnRotation);
		}
	}
	
	if (IsValid(SpawnHandler))
	{
		SpawnHandler->AsyncSpawn();
	}
}

void USpawnManager::SpawnWave(const UEnemyGroupData* EnemyGroupData)
{
	if (!IsValid(EnemyGroupData))
	{
		UE_LOG(LogSpawnManagerSubsystem, Error, TEXT("SpawnWave called with null EnemyGroupData"));
		return;
	}
	
	for (const FEnemyGroup& EnemyGroup : EnemyGroupData->EnemyGroups)
	{
		SpawnGroup(EnemyGroup);
	}

	if (AsyncSpawnHandlerCount == 0)
	{
		OnAllGroupsSpawned.Broadcast();
	}
}

void USpawnManager::SpawnWave(const UWaveData* WaveData, const int32 WaveIndex)
{
	if (!IsValid(WaveData))
	{
		UE_LOG(LogSpawnManagerSubsystem, Error, TEXT("SpawnWave called with null WaveData"));
		return;
	}

	if (!WaveData->Waves.IsValidIndex(WaveIndex))
	{
		UE_LOG(LogSpawnManagerSubsystem, Error, TEXT("SpawnWave called with invalid WaveIndex %d"), WaveIndex);
		return;
	}

	SpawnWave(WaveData->Waves[WaveIndex].EnemyGroupData);
}

void USpawnManager::StopPendingSpawnRequests() const 
{
	OnStopPendingSpawnRequests.Broadcast();
}

void USpawnManager::StopPendingSpawnRequests(const UWaveData* WaveData, const int32 WaveIndex)
{
	StopPendingSpawnRequests();
}

void USpawnManager::AsyncSpawnCompleted()
{
	AsyncSpawnHandlerCount--;
	
	if (AsyncSpawnHandlerCount <= 0)
	{
		AsyncSpawnHandlerCount = 0;
		OnAllGroupsSpawned.Broadcast();
	}
}

bool USpawnManager::IsRangeZero(const FFloatRange& Range) const
{
	return (!Range.HasLowerBound() || Range.GetLowerBoundValue() <= 0.1f) &&
		   (!Range.HasUpperBound() || Range.GetUpperBoundValue() <= 0.1f);
}

int32 USpawnManager::GetCountFromRange(const FInt32Range& Range) const
{
	int32 MinRange = 0;
	int32 MaxRange = 0;

	if (Range.HasLowerBound() && Range.GetLowerBoundValue() > 0)
	{
		MinRange = Range.GetLowerBoundValue();
	}

	if (Range.HasUpperBound() && Range.GetUpperBoundValue() > 0)
	{
		MaxRange = Range.GetUpperBoundValue();
	}
	
	return FMath::RandRange(MinRange, MaxRange);
}
