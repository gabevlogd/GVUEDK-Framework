// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "BlueprintTriggerActionFactory.generated.h"

/**
 * 
 */
UCLASS()
class WORLDTRIGGERSYSTEMEDITOR_API UBlueprintTriggerActionFactory : public UFactory
{
	GENERATED_BODY()

public:

	UBlueprintTriggerActionFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
