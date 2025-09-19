// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "ResourceAttributeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FIncreasedAttribute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDecreasedAttribute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReachedMaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReachedMinValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMaxValueChanged);

UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew)
class RESOURCEATTRIBUTESYSTEM_API UResourceAttributeBase : public UObject
{
	GENERATED_BODY()

public:
	UResourceAttributeBase();
	virtual void Initialize(AActor* InOwner, const FGameplayTag InTag);

	UFUNCTION(BlueprintCallable)
	float GetCurrentValue() const { return CurrentValue; }
	
	UFUNCTION(BlueprintCallable)
	virtual void SetValue(const float NewValue);
	
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Returns true if the value was secsessfully added"))
	virtual bool AddValue(const float Value);
	
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Returns true if the value was secsessfully removed"))
	virtual bool RemoveValue(const float Value);
	
	UFUNCTION(BlueprintCallable)
	virtual bool IsFull() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetMaxValue(const float NewMaxValue);

private:
	void StartRegen();
	void StopRegen();
	void Regen();
	void CheckOrder(float& InMinValue, float& InMaxValue);

public:
	UPROPERTY(BlueprintAssignable)
	FIncreasedAttribute OnIncreasedAttribute;
	
	UPROPERTY(BlueprintAssignable)
	FDecreasedAttribute OnDecreasedAttribute;
	
	UPROPERTY(BlueprintAssignable)
	FReachedMaxValue OnReachedMaxValue;
	
	UPROPERTY(BlueprintAssignable)
	FReachedMinValue OnReachedMinValue;

	UPROPERTY(BlueprintAssignable)
	FMaxValueChanged OnMaxValueChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRegen;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bCanRegen"))
	bool bStopRegenOnValueDecrease;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "If true, the attribute will start with the StartingValue instead of the MaxValue"))
	bool bUseStartingValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseStartingValue"))
	float StartingValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bCanRegen"))
	float RegenSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bCanRegen"))
	float RegenDelay;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bCanRegen", ToolTip = "Regen starts when the current value is below this threshold"))
	float StartRegenValue;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bCanRegen", ToolTip = "Regen stops when the current value is above this threshold"))
	// float StopRegenValue;

protected:
	UPROPERTY()
	AActor* Owner;
	
	FGameplayTag Tag;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentValue;

private:
	bool bMustRegen;
	FTimerHandle RegenTimer;
	FTimerHandle RegenDelayTimer;
	

	
};
