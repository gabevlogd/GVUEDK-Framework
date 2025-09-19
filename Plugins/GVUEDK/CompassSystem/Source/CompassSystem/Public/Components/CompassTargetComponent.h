// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CompassTargetComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMPASSSYSTEM_API UCompassTargetComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	UObject* TargetIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	bool bHideIconIfOutOfSight = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Compass", meta=(AllowPrivateAccess="true"))
	bool bAutoAddToCompass = false;

public:
	
	UCompassTargetComponent();

	FVector GetLocation() const { return GetOwner()->GetActorLocation(); }

	UObject* GetTargetIcon() const { return TargetIcon; }

	bool ShouldHideIconIfOutOfSight() const { return bHideIconIfOutOfSight; }

	FName GetTargetName() const { return GetOwner()->GetFName(); }

protected:
	
	virtual void BeginPlay() override;
};
