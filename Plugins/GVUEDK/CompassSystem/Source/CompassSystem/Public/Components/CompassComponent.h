// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CompassTargetComponent.h"
#include "Components/ActorComponent.h"
#include "Widgets/CompassWidget.h"
#include "CompassComponent.generated.h"

UENUM(BlueprintType)
enum class ECardinalPoint : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	North = 1 UMETA(DisplayName = "North"),
	NorthEast = 2 UMETA(DisplayName = "North East"),
	NorthWest = 3 UMETA(DisplayName = "North West"),
	South = 4 UMETA(DisplayName = "South"),
	SouthEast = 5 UMETA(DisplayName = "South East"),
	SouthWest = 6 UMETA(DisplayName = "South West"),
	East = 7 UMETA(DisplayName = "East"),
	West = 8 UMETA(DisplayName = "West")
};

const TMap<ECardinalPoint, FVector> CardinalDirections =
{
	{ECardinalPoint::North, FVector::ForwardVector, },
	{ECardinalPoint::NorthEast, (FVector::ForwardVector + FVector::RightVector).GetSafeNormal()},
	{ECardinalPoint::NorthWest, (FVector::ForwardVector + FVector::LeftVector).GetSafeNormal()},
	{ECardinalPoint::South, FVector::BackwardVector},
	{ECardinalPoint::SouthEast, (FVector::BackwardVector + FVector::RightVector).GetSafeNormal()},
	{ECardinalPoint::SouthWest, (FVector::BackwardVector + FVector::LeftVector).GetSafeNormal()},
	{ECardinalPoint::East, FVector::RightVector},
	{ECardinalPoint::West, FVector::LeftVector}
};

USTRUCT(Blueprintable, BlueprintType)
struct FCardinalPoint
{
	GENERATED_BODY()

	FCardinalPoint()
	{
		Name = ECardinalPoint::None;
		Icon = nullptr;
	}

	FCardinalPoint(const ECardinalPoint InName, UObject* InIcon)
	{
		Name = InName;
        Icon = InIcon;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transitions")
	ECardinalPoint Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transitions")
	UObject* Icon;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMPASSSYSTEM_API UCompassComponent : public UActorComponent
{
	GENERATED_BODY()

	
private:

	UPROPERTY(BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	UCompassWidget* CompassWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	UObject* NorthIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	UObject* NorthEastIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	UObject* NorthWestIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	UObject* SouthIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	UObject* SouthEastIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	UObject* SouthWestIcon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	UObject* EastIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	UObject* WestIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	float CardinalPointOffsetDegrees = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	bool bOrientCompassWidget = false;

	UPROPERTY()
	TSet<UCompassTargetComponent*> CompassTargets;

	TArray<FCardinalPoint> CardinalPoints;

	UPROPERTY()
	APlayerController* PlayerController;

	bool bInitialized = false;

public:
	
	UCompassComponent();

	UFUNCTION(BlueprintCallable, Category="Compass")
	void Init(UCompassWidget* InCompassWidget);

	UFUNCTION(BlueprintCallable, Category="Compass")
	bool AddTarget(UCompassTargetComponent* CompassTarget);

	UFUNCTION(BlueprintCallable, Category="Compass")
	bool RemoveTarget(const UCompassTargetComponent* CompassTarget);

	void OrientCompassWidget(float Orientation);

private:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateCompassTargets();

	void UpdateCardinalPoints();

	float GetIconPosition(const FVector& TargetLocation);

	void InitCardinalPoints();

};
