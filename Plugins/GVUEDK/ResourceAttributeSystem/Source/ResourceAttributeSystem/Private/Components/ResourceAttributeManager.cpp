// Copyright Villains, Inc. All Rights Reserved.


#include "Components/ResourceAttributeManager.h"

#include "Base/ResourceAttributeBase.h"


UResourceAttributeManager::UResourceAttributeManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UResourceAttributeManager::BeginPlay()
{
	Super::BeginPlay();
	InitializeAttributes();
}

bool UResourceAttributeManager::TryGetAttribute(const FGameplayTag AttributeTag, UResourceAttributeBase*& OutAttribute) const
{
	if (AttributeMap.Contains(AttributeTag))
	{
		OutAttribute = AttributeMap[AttributeTag];
		return true;
	}
	OutAttribute = nullptr;
	return false;
}

void UResourceAttributeManager::InitializeAttributes()
{
	for (const auto& Elem : AttributeMap)
	{
		Elem.Value->Initialize(GetOwner(), Elem.Key);
	}
}

