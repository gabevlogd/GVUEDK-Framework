// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/ResourceAttributeBase.h"
#include "HealthAttribute.generated.h"

/**
 * 
 */
UCLASS()
class RESOURCEATTRIBUTESYSTEM_API UHealthAttribute : public UResourceAttributeBase
{
	GENERATED_BODY()

public:

	virtual void Initialize(AActor* InOwner, const FGameplayTag InTag) override;

	virtual void SetValue(const float NewValue) override;

	virtual bool RemoveValue(const float Value) override;
	
	UFUNCTION(BlueprintCallable)
	bool IsDead() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsInvulnerable() const;
	
	UFUNCTION(BlueprintCallable)
	void SetInvulnerable(const bool Value);

private:
	bool bIsInvulnerable;
};
