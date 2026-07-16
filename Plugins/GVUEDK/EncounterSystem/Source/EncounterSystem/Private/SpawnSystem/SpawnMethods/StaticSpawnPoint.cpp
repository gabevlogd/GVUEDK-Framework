#include "SpawnSystem/SpawnMethods/StaticSpawnPoint.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"


bool UStaticSpawnPoint::Init(USpawnMethod* Template, USpawnManager* InSpawnManager)
{
	if (!Super::Init(Template, InSpawnManager)) return false;

	if (const UStaticSpawnPoint* StaticSpawnPoint = Cast<UStaticSpawnPoint>(Template))
	{
		SpawnPointLocation = StaticSpawnPoint->SpawnPointLocation;
		SpawnRotation = StaticSpawnPoint->SpawnRotation;
		NavMeshSearchExtent = StaticSpawnPoint->NavMeshSearchExtent;
		MinSpawnDistance = StaticSpawnPoint->MinSpawnDistance;
		return true;
	}

	UE_LOG(LogSpawnMethodFactory, Error, TEXT("UStaticSpawnPoint::Init called with invalid template, return false"));;
	return false;
}

bool UStaticSpawnPoint::FindSpawnLocation(UWorld* World, AActor* ContextActor, FVector& OutLocation)
{
	// if (!IsValid(World)) return false;
	//
	// UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
	// if (!IsValid(NavSys)) return false;
	// 	
	// FNavLocation NavLocation;
	//
	// // Try multiple times to find a valid navmesh location near the spawn point (max 10 attempts)
	// constexpr int32 MaxAttempts = 10;
	// for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt)
	// {
	// 	// 1. Random offset within extent
	// 	FVector RandomOffset = FVector(
	// 		FMath::FRandRange(-NavMeshSearchExtent.X, NavMeshSearchExtent.X),
	// 		FMath::FRandRange(-NavMeshSearchExtent.Y, NavMeshSearchExtent.Y),
	// 		FMath::FRandRange(-NavMeshSearchExtent.Z, NavMeshSearchExtent.Z)
	// 	);
	//
	// 	FVector TestLocation = SpawnPointLocation + RandomOffset;
	//
	// 	// 2. Project to navmesh
	// 	if (!NavSys->ProjectPointToNavigation(TestLocation, NavLocation, NavMeshSearchExtent))
	// 		continue;
	//
	// 	// 3. Check for overlaps at the location
	// 	//if (IsLocationOverlapping(World, NavLocation.Location))
	// 		//continue;
	// 	
	// 	OutLocation = NavLocation.Location;
	// 	return true;
	// }
	// return false;
	OutLocation = SpawnPointLocation;
	return true;
}

bool UStaticSpawnPoint::FindSpawnRotation(UWorld* World, AActor* ContextActor, FRotator& OutRotation)
{
	OutRotation = (UGameplayStatics::GetPlayerPawn(World, 0)->GetActorLocation() - SpawnPointLocation).Rotation();
	return true;
}

void UStaticSpawnPoint::DrawGizmo(FPrimitiveDrawInterface* PDI)
{
	DrawWireSphere( PDI, SpawnPointLocation, FLinearColor::Red, 32.f, 24, SDPG_Foreground);
}

bool UStaticSpawnPoint::IsLocationOverlapping(UWorld* World, FVector Location)
{
	if (!IsValid(World)) return false;

	Location += FVector(0.f, 0.f, NavMeshSearchExtent.Z); // Adjust for box center

	// Check against previously occupied locations, using squared distance for efficiency
	for (FVector Element : PreviousLocations)
	{
		if (FVector::DistSquared(Location, Element) < FMath::Square(MinSpawnDistance))
		{
			return true;
		}
	}
	
	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(NavMeshSearchExtent);
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	
	// Perform overlap test
	const bool bResult = World->OverlapAnyTestByChannel(
		Location,
		FQuat::Identity,
		ECC_Visibility,
		CollisionShape,
		QueryParams);

	if (!bResult)
		PreviousLocations.Add(Location);

#if WITH_EDITOR
	if (bShowDebug)
		DrawDebugBox(World, Location, NavMeshSearchExtent, bResult ? FColor::Red : FColor::Green, false, 2.f);
#endif
	
	return bResult;
}
