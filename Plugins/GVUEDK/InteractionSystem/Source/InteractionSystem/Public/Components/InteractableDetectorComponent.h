// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableDetectorComponent.generated.h"


class UInteractableDetectorBehaviourBase;
class IInteractable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableDetected, TScriptInterface<IInteractable>, Interactable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractableLost);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERACTIONSYSTEM_API UInteractableDetectorComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, meta = (AllowPrivateAccess = "true"))
	UInteractableDetectorBehaviourBase* DetectorBehaviour;

	TScriptInterface<IInteractable> LastDetectedInteractable;

public:
	UPROPERTY(BlueprintAssignable)
	FInteractableDetected OnInteractableDetected;
	UPROPERTY(BlueprintAssignable)
	FInteractableLost OnInteractableLost;
	
	UInteractableDetectorComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	bool TryGetInteractable(TScriptInterface<IInteractable>& OutInteractable) const;

private:
	void CheckInteractablesInRange();
};
