// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "ResourceAttributeManager.generated.h"


class UResourceAttributeBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RESOURCEATTRIBUTESYSTEM_API UResourceAttributeManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UResourceAttributeManager();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	bool TryGetAttribute(const FGameplayTag AttributeTag, UResourceAttributeBase*& OutAttribute) const;

private:
	void InitializeAttributes();

private:
	UPROPERTY(EditAnywhere, Instanced, meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, UResourceAttributeBase*> AttributeMap;
};
