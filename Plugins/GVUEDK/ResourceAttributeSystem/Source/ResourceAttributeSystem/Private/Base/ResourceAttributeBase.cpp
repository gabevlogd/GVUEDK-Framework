// Copyright Villains, Inc. All Rights Reserved.


#include "Base/ResourceAttributeBase.h"

UResourceAttributeBase::UResourceAttributeBase(): bCanRegen(false), bStopRegenOnValueDecrease(true),
                                                  MaxValue(1), MinValue(0), bUseStartingValue(false), StartingValue(1),
                                                  RegenSpeed(1),
                                                  RegenDelay(1), StartRegenValue(1 - KINDA_SMALL_NUMBER),
                                                  CurrentValue(1), bMustRegen(false)
{
	Owner = nullptr;
}

void UResourceAttributeBase::Initialize(AActor* InOwner, const FGameplayTag InTag)
{
	if (!IsValid(InOwner))
	{
		UE_LOG(LogTemp, Error, TEXT("%s needs a valid owner, initialization failed."), *GetName());
		return;
	}
	Owner = InOwner;
	Tag = InTag;

	CheckOrder(MinValue, MaxValue);
	
	if (bCanRegen)
	{
		//CheckOrder(StartRegenValue, StopRegenValue);
		
		if (!FMath::IsWithinInclusive(StartRegenValue, MinValue, MaxValue))
		{
			UE_LOG(LogTemp, Warning, TEXT("Trigger regen value is not within the min and max value range, setting it to the min value."));
			StartRegenValue = MinValue;
		}
		// if (!FMath::IsWithinInclusive(StopRegenValue, MinValue, MaxValue))
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Stop regen value is not within the min and max value range, setting it to the max value."));
		// 	StopRegenValue = MaxValue;
		// }
	}

	if (bUseStartingValue)
		SetValue(StartingValue);
	else
		SetValue(MaxValue);
}

void UResourceAttributeBase::SetValue(const float NewValue)
{
	//if (CurrentValue == NewValue) return;

	const float ValueBeforeChange = CurrentValue;

	if (CurrentValue < NewValue)
	{
		CurrentValue = FMath::Min(NewValue, MaxValue);
		OnIncreasedAttribute.Broadcast();
		if (CurrentValue == MaxValue && ValueBeforeChange != MaxValue)
		{
			OnReachedMaxValue.Broadcast();
		}
		return;
	}
	
	CurrentValue = FMath::Max(NewValue, MinValue);
	OnDecreasedAttribute.Broadcast();
	if (CurrentValue == MinValue && ValueBeforeChange != MinValue)
	{
		OnReachedMinValue.Broadcast();
	}
	
	if (bCanRegen)
	{
		if (bStopRegenOnValueDecrease && bMustRegen)
		{
			StopRegen();
		}
		if (!bMustRegen /*&& CurrentValue <= StartRegenValue*/)
		{
			StartRegen();
		}
	}
	
}

bool UResourceAttributeBase::AddValue(const float Value)
{
	if (CurrentValue == MaxValue) return false;
	const float AbsValue = FMath::Abs(Value);	
	SetValue(CurrentValue + AbsValue);
	return true;
}

bool UResourceAttributeBase::RemoveValue(const float Value)
{
	if (CurrentValue == MinValue) return false;
	const float AbsValue = FMath::Abs(Value);
	SetValue(CurrentValue - AbsValue);
	return true;
}

bool UResourceAttributeBase::IsFull() const
{
	return CurrentValue == MaxValue;
}

void UResourceAttributeBase::SetMaxValue(const float NewMaxValue)
{
	MaxValue = FMath::Max(NewMaxValue, MinValue + KINDA_SMALL_NUMBER);
	if (CurrentValue > MaxValue)
	{
		CurrentValue = MaxValue;
	}
	else if (bCanRegen && !bMustRegen /*&& CurrentValue <= StartRegenValue*/)
	{
		StartRegen();
	}
	OnMaxValueChanged.Broadcast();
}

void UResourceAttributeBase::StartRegen()
{
	bMustRegen = true;
	
	
	if (RegenDelay <= 0)
	{
		Regen();
		return;
	}
	
	// Start regen after delay
	//FTimerHandle TimerHandle;
	Owner->GetWorld()->GetTimerManager().ClearTimer(RegenDelayTimer);
	const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UResourceAttributeBase::Regen);
	Owner->GetWorld()->GetTimerManager().SetTimer(RegenDelayTimer, TimerDelegate, RegenDelay, false);
}

void UResourceAttributeBase::StopRegen()
{
	bMustRegen = false;
	Owner->GetWorld()->GetTimerManager().ClearTimer(RegenTimer);
}

void UResourceAttributeBase::Regen()
{
	if (!bMustRegen) return;

	if (CurrentValue >= MaxValue)
	{
		CurrentValue = MaxValue;
		StopRegen();
		return;
	}

	const float DeltaTime = Owner->GetWorld()->GetDeltaSeconds();
	AddValue(DeltaTime * RegenSpeed);

	const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UResourceAttributeBase::Regen);
	RegenTimer = Owner->GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
}

void UResourceAttributeBase::CheckOrder(float& InMinValue, float& InMaxValue)
{
	if (InMaxValue < InMinValue)
	{
		const float Swap = InMaxValue;
		InMaxValue = InMinValue;
		InMinValue = Swap;
		UE_LOG(LogTemp, Warning, TEXT("Max value is smaller than min value, swapping values."));
	}
	else if (InMaxValue == InMinValue)
	{
		InMaxValue++;
		UE_LOG(LogTemp, Warning, TEXT("Max value is equal to min value, increasing max value by 1."));
	}
}
