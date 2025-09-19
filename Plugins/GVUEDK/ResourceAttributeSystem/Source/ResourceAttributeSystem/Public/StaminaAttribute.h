// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/ResourceAttributeBase.h"
#include "StaminaAttribute.generated.h"

/**
 * 
 */
UCLASS()
class RESOURCEATTRIBUTESYSTEM_API UStaminaAttribute : public UResourceAttributeBase
{
	GENERATED_BODY()

public:

	virtual void Initialize(AActor* InOwner, const FGameplayTag InTag) override;
};
