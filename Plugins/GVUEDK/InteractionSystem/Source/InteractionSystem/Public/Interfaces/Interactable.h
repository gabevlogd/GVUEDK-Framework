// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INTERACTIONSYSTEM_API IInteractable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StopInteract();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Highlight();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Unhighlight();
	
	/**
	* @brief Check if the interactable can be interacted
	* @param Caller The actor that is trying to interact with the interactable
	* @return True if the interactable can be interacted
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanBeInteracted(AActor* Caller) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetInteractableLocation() const;
};
