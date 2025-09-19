// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMesh.h"
#include "Components/SplineComponent.h"
#include "GrindableRail.generated.h"

UCLASS()
class CUSTOMMOVEMENTSYSTEM_API AGrindableRail : public AActor
{
	GENERATED_BODY()

public:
	
	AGrindableRail();

	USplineComponent* GetSplineComponent() const { return Spline; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail")
	UStaticMesh* RailMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail")
	bool bStretchMesh = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail")
	FVector MeshRelativeOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail")
	bool bGrindOnCollision = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail")
	USplineComponent* Spline;

	virtual void OnConstruction(const FTransform& Transform) override;

private:

	int GetLastIndex() const;
	FVector GetMeshExtent() const;
	void GetStartAndEnd(const int PointIndex, FVector& StartLocation, FVector& StartTangent, FVector& EndLocation, FVector& EndTangent) const;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	
};
