// Copyright Villains, Inc. All Rights Reserved.


#include "StaminaAttribute.h"

void UStaminaAttribute::Initialize(AActor* InOwner, const FGameplayTag InTag)
{
	Super::Initialize(InOwner, InTag);

	if (!Tag.IsValid())
	{
		Tag = FGameplayTag::RequestGameplayTag(FName("ResourceAttribute.Stamina"));
	}
}
