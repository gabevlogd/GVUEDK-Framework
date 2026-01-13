// Copyright Villains, Inc. All Rights Reserved.


#include "SpawnSystem/AsyncSpawnHandler.h"

UAsyncSpawnHandler::UAsyncSpawnHandler()
{
	SpawnManager = nullptr;
}

void UAsyncSpawnHandler::Init(USpawnManager* InSpawnManager, const FFloatRange InSpawnDelayRange, const FFloatRange InSpawnIntervalRange)
{
	SpawnManager = InSpawnManager;

	if (IsValid(SpawnManager))
	{
		SpawnManager->OnStopPendingSpawnRequests.AddUniqueDynamic(this, &UAsyncSpawnHandler::Deinitialize);
	}

	SpawnTimer = FTimerHandle();
	SpawnDelegateHandle = FTimerDelegate::CreateUObject(this, &UAsyncSpawnHandler::AsyncSpawn);
	SpawnDelay = GetValidFloatInRange(InSpawnDelayRange);
	SpawnIntervalRange = InSpawnIntervalRange;

	WorldCleanupDelegateHandle = FWorldDelegates::OnWorldCleanup.AddUObject(this, &UAsyncSpawnHandler::OnWorldCleanup);
}

void UAsyncSpawnHandler::Deinitialize() 
{
	if (IsValid(SpawnManager))
	{
		SpawnManager->OnStopPendingSpawnRequests.RemoveDynamic(this, &UAsyncSpawnHandler::Deinitialize);
		
		if (IsValid(SpawnManager->GetWorld()))
			SpawnManager->GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
	}
	FWorldDelegates::OnWorldCleanup.Remove(WorldCleanupDelegateHandle);
	SpawnTimer.Invalidate();
	SpawnDelegateHandle.Unbind();
	SpawnRequests.Empty();
	SpawnManager->AsyncSpawnCompleted();
	SpawnManager = nullptr;
	RemoveFromRoot();
}

void UAsyncSpawnHandler::OnWorldCleanup(UWorld* World, bool bArg, bool bCond)
{
	Deinitialize();
}

float UAsyncSpawnHandler::GetValidFloatInRange(FFloatRange InRange)
{
	float MinRange = 0.f;
	float MaxRange = 0.f;

	if (InRange.HasLowerBound() && InRange.GetLowerBoundValue() > 0.f)
	{
		MinRange = InRange.GetLowerBoundValue();
	}

	if (InRange.HasUpperBound() && InRange.GetUpperBoundValue() > 0.f)
	{
		MaxRange = InRange.GetUpperBoundValue();
	}

	if (MaxRange <= KINDA_SMALL_NUMBER && MinRange <= KINDA_SMALL_NUMBER)
	{
		return -1.f;
	}
	
	return FMath::FRandRange(MinRange, MaxRange);
}

void UAsyncSpawnHandler::AsyncSpawn()
{
	if (SpawnRequests.Num() == 0 || !IsValid(SpawnManager) || !IsValid(SpawnManager->GetWorld()))
	{
		Deinitialize();
		return;
	}

	if (SpawnDelay > 0.f)
	{
		SpawnManager->GetWorld()->GetTimerManager().SetTimer(SpawnTimer, SpawnDelegateHandle, SpawnDelay, false);
		SpawnDelay = -1.f;
		return;
	}

	const FSpawnRequest SpawnRequest = SpawnRequests[0];
	SpawnRequests.RemoveAt(0);

	AActor* Enemy = SpawnManager->SpawnEnemyAtLocation(SpawnRequest.EnemyClass, SpawnRequest.Location, SpawnRequest.Rotation);

	if (SpawnRequests.Num() > 0)
	{
		const float SpawnInterval = GetValidFloatInRange(SpawnIntervalRange);
		if (SpawnInterval <= KINDA_SMALL_NUMBER)
		{
			AsyncSpawn();
			return;
		}
		SpawnManager->GetWorld()->GetTimerManager().SetTimer(SpawnTimer, SpawnDelegateHandle, SpawnInterval, false);
		return;
	}

	Deinitialize();
}

void UAsyncSpawnHandler::AddSpawnRequest(const FSpawnRequest& SpawnRequest)
{
	SpawnRequests.Add(SpawnRequest);
}


