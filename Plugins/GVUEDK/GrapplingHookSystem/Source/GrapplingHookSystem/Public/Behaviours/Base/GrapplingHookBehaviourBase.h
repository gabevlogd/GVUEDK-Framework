// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GrapplingHookBehaviourBase.generated.h"

class IGrabPoint;
class UGrapplingHookComponent;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew)
class GRAPPLINGHOOKSYSTEM_API UGrapplingHookBehaviourBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGrapplingHookComponent* InGrapplingHookComponent);

	// Returns true if the tick of the GrapplingHookComponent should be stopped
	virtual bool TickMode(float DeltaTime) { return false; }
	virtual void EnterMode();
	virtual void ExitMode();

protected:
	IGrabPoint* GetTargetGrabPoint() const;
	void SetTargetGrabPoint(IGrabPoint* GrabPoint);
	bool IsTargetAcquired() const;
	UPROPERTY()
	UGrapplingHookComponent* GrapplingHookComponent;
	UPROPERTY()
	ACharacter* OwnerCharacter;
};
