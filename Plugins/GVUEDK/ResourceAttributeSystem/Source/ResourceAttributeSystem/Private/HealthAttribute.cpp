// Copyright Villains, Inc. All Rights Reserved.


#include "HealthAttribute.h"


void UHealthAttribute::Initialize(AActor* InOwner, const FGameplayTag InTag)
{
	Super::Initialize(InOwner, InTag);

	if (!Tag.IsValid())
	{
		Tag = FGameplayTag::RequestGameplayTag(FName("ResourceAttribute.Health"));
	}
}

void UHealthAttribute::SetValue(const float NewValue)
{
	// If the health is decreasing and the attribute is invulnerable, do not change the value
	if (NewValue < CurrentValue && IsInvulnerable())
	{
		return;
	}
	Super::SetValue(NewValue);
}

bool UHealthAttribute::RemoveValue(const float Value)
{
	if (IsInvulnerable()) return false;
	return Super::RemoveValue(Value);
}

bool UHealthAttribute::IsDead() const
{
	return CurrentValue <= 0.f;
}

bool UHealthAttribute::IsInvulnerable() const
{
	return bIsInvulnerable;
}

void UHealthAttribute::SetInvulnerable(const bool Value)
{
	bIsInvulnerable = Value;
}
