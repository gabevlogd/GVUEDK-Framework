// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "BlueprintTriggerConditionFactory.generated.h"

/**
 * 
 */
UCLASS()
class WORLDTRIGGERSYSTEMEDITOR_API UBlueprintTriggerConditionFactory : public UFactory
{
	GENERATED_BODY()

public:

	UBlueprintTriggerConditionFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
