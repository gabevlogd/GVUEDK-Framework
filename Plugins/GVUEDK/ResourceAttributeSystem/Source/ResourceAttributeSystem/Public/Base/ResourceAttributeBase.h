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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FValueChangedImmediately);

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

	// Sets the value without any checks or clamps and without triggering any of the increase/decrease or max/min reached events, use with caution
	UFUNCTION(BlueprintCallable)
	virtual void SetValueImmediately(const float NewValue);
	
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Returns true if the value was secsessfully added"))
	virtual bool AddValue(const float Value);
	
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Returns true if the value was secsessfully removed"))
	virtual bool RemoveValue(const float Value);
	
	UFUNCTION(BlueprintCallable)
	virtual bool IsFull() const;

	UFUNCTION(BlueprintCallable)
	virtual bool IsEmpty() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetMaxValue(const float NewMaxValue);

	UFUNCTION(BlueprintCallable)
	virtual void SetCanUpdateValue(const bool NewCanUpdateValue);

	UFUNCTION(BlueprintCallable)
	virtual bool CanUpdateValue();

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

	UPROPERTY(BlueprintAssignable)
	FValueChangedImmediately OnValueChangedImmediately;

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

protected:
	UPROPERTY()
	AActor* Owner;
	
	FGameplayTag Tag;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentValue;

private:
	bool bMustRegen;
	bool bCanUpdateValue;
	FTimerHandle RegenTimer;
	FTimerHandle RegenDelayTimer;
	

	
};
