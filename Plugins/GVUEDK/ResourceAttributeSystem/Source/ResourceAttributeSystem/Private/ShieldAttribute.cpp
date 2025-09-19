// Copyright Villains, Inc. All Rights Reserved.


#include "ShieldAttribute.h"

void UShieldAttribute::SetValue(const float NewValue)
{
	if (IsInvulnerable()) return;
	Super::SetValue(NewValue);
}

bool UShieldAttribute::IsInvulnerable() const
{
	return bIsInvulnerable;
}

void UShieldAttribute::SetInvulnerable(const bool Value)
{
	bIsInvulnerable = Value;
}
