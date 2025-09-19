// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/InteractableDetectorBehaviourBase.h"
#include "SphereDetectionBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIONSYSTEM_API USphereDetectionBehaviour : public UInteractableDetectorBehaviourBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxDetectionDistance = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 1.f))
	float MaxDetectionAngle = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bShowDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", EditCondition = "bShowDebug"))
	float InteractableSphereRadius = 50.f;

public:
	UFUNCTION(BlueprintCallable)
	virtual bool TryGetInteractable(TScriptInterface<IInteractable>& OutInteractable) const override;
	
private:
	TScriptInterface<IInteractable> GetMostRelevantInteractable(TArray<TScriptInterface<IInteractable>> Interactables) const;
	FVector GetOwnerCameraLocation() const;
};
