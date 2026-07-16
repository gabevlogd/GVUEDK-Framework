// Copyright Villains, Inc. All Rights Reserved.


#include "Factories/BlueprintTriggerConditionFactory.h"

#include "Kismet2/KismetEditorUtilities.h"
#include "TriggerConditions/BlueprintTriggerCondition.h"

UBlueprintTriggerConditionFactory::UBlueprintTriggerConditionFactory()
{
	SupportedClass = UBlueprintTriggerCondition::StaticClass();
	bCreateNew = true;
}

UObject* UBlueprintTriggerConditionFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FKismetEditorUtilities::CreateBlueprint(
		SupportedClass,
		InParent,
		InName,
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass(),
		FName("BlueprintTriggerConditionFactory")
	);
}
