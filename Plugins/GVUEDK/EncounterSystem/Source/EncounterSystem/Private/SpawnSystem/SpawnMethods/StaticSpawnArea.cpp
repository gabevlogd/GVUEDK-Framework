#include "SpawnSystem/SpawnMethods/StaticSpawnArea.h"
#include "NavigationSystem.h"

bool UStaticSpawnArea::Init(USpawnMethod* Template, USpawnManager* InSpawnManager)
{
	if (!Super::Init(Template, InSpawnManager)) return false;

	if (const UStaticSpawnArea* StaticSpawnArea = Cast<UStaticSpawnArea>(Template))
	{
		Width = StaticSpawnArea->Width;
		Depth = StaticSpawnArea->Depth;
		Height = StaticSpawnArea->Height;
		return true;
	}

	UE_LOG(LogSpawnMethodFactory, Error, TEXT("UStaticSpawnArea::Init called with invalid template, return false"));;
	return false;
}

bool UStaticSpawnArea::FindSpawnLocation(UWorld* World, AActor* ContextActor, FVector& OutLocation)
{
	if (!IsValid(World)) return false;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
	if (!IsValid(NavSys)) return false;
		
	FNavLocation NavLocation;
	const FBox Area = GetAreaBox();

	// Try multiple times to find a valid navmesh location inside the spawn area (max 10 attempts)
	constexpr int32 MaxAttempts = 10;
	for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt)
	{
		// 1. Random point inside the spawn area
		FVector TestLocation = FMath::RandPointInBox(Area);

		// 2. Project to navmesh
		if (!NavSys->ProjectPointToNavigation(TestLocation, NavLocation, NavMeshSearchExtent))
			continue;

		// 3. Check if the projected location is still inside the area
		if (!Area.IsInside(NavLocation.Location))
			continue;

		// 4. Check for overlaps at the location
		if (IsLocationOverlapping(World, NavLocation.Location))
			continue;

		OutLocation = NavLocation.Location;
		return true;
	}
		
	return false;
}

void UStaticSpawnArea::DrawGizmo(FPrimitiveDrawInterface* PDI)
{
	// Draw the box
	DrawWireBox(PDI, GetAreaBox(), FLinearColor::Red, SDPG_Foreground);
}

FBox UStaticSpawnArea::GetAreaBox() const
{
	// half-size
	const FVector Extent(Width * 0.5f, Depth * 0.5f, Height * 0.5f);

	// Central point of the box
	const FVector BoxCenter = SpawnPointLocation + FVector(0, 0, Extent.Z);

	// Create the box
	FBox Box(BoxCenter - Extent, BoxCenter + Extent);

	return Box;
}
