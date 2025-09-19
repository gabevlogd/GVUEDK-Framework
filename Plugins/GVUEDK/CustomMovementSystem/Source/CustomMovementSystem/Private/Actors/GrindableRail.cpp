// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GrindableRail.h"

#include "Components/GvCharacterMovementComponent.h"
#include "Components/SplineMeshComponent.h"


AGrindableRail::AGrindableRail()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("RailSpline"));
	SetRootComponent(Spline);
	Spline->SetMobility(EComponentMobility::Static);
}

void AGrindableRail::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!Spline) return;

	for (int i = 0; i <= GetLastIndex(); i++)
	{
		USplineMeshComponent* NewMesh = static_cast<USplineMeshComponent*>(AddComponentByClass(USplineMeshComponent::StaticClass(), false, FTransform(), false));
		NewMesh->SetStaticMesh(RailMesh);
		FVector StartLocation, StartTangent, EndLocation, EndTangent;
		GetStartAndEnd(i, StartLocation, StartTangent, EndLocation, EndTangent);
		NewMesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);
		NewMesh->AddRelativeLocation(MeshRelativeOffset);
		NewMesh->SetSplineUpDir(Spline->GetUpVectorAtSplinePoint(i, ESplineCoordinateSpace::Local));
		if (bGrindOnCollision)
		{
			NewMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
			NewMesh->SetGenerateOverlapEvents(true);
			NewMesh->OnComponentBeginOverlap.AddUniqueDynamic(this, &AGrindableRail::OnOverlapBegin);
		}
		else
		{
			NewMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		}
	}
}

int AGrindableRail::GetLastIndex() const
{
	if (!Spline) return -1;
	if (bStretchMesh)
	{
		return Spline->GetNumberOfSplinePoints() - 2.f;
	}
	else
	{
		return FMath::TruncToInt(Spline->GetSplineLength() / GetMeshExtent().X);
	}
}

FVector AGrindableRail::GetMeshExtent() const
{
	if (!RailMesh) return FVector::ZeroVector;
	return RailMesh->GetBoundingBox().Min.GetAbs() + RailMesh->GetBoundingBox().Max.GetAbs();
}

void AGrindableRail::GetStartAndEnd(const int PointIndex, FVector& StartLocation, FVector& StartTangent, FVector& EndLocation,
	FVector& EndTangent) const
{
	if (!Spline) return;
	
	if (bStretchMesh)
	{
		Spline->GetLocationAndTangentAtSplinePoint(PointIndex, StartLocation, StartTangent, ESplineCoordinateSpace::Local);
		Spline->GetLocationAndTangentAtSplinePoint(PointIndex + 1, EndLocation, EndTangent, ESplineCoordinateSpace::Local);
	}
	else
	{
		float StartDistance = GetMeshExtent().X * PointIndex;
		StartLocation = Spline->GetLocationAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::Local);
		StartTangent = Spline->GetTangentAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::Local).GetClampedToSize(0.f, GetMeshExtent().X);

		float EndDistance = GetMeshExtent().X * PointIndex + GetMeshExtent().X;
		EndLocation = Spline->GetLocationAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::Local);
		EndTangent = Spline->GetTangentAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::Local).GetClampedToSize(0.f, GetMeshExtent().X);
	}
}

void AGrindableRail::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UGvCharacterMovementComponent* MovementComponent = Cast<UGvCharacterMovementComponent>(OtherActor->GetComponentByClass(UGvCharacterMovementComponent::StaticClass()));
	if (!MovementComponent) return;
	if (MovementComponent->IsRailGrinding()) return;
	if (MovementComponent->TryRailGrind(Spline))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Rail Grind started"));
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Rail Grind failed"));
	}
}

