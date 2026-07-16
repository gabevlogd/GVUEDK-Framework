// Copyright Villains, Inc. All Rights Reserved.


#include "Factories/BlueprintTriggerActionFactory.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "TriggerActions/BlueprintTriggerAction.h"

UBlueprintTriggerActionFactory::UBlueprintTriggerActionFactory()
{
	SupportedClass = UBlueprintTriggerAction::StaticClass();
	bCreateNew = true;
}

UObject* UBlueprintTriggerActionFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FKismetEditorUtilities::CreateBlueprint(
		SupportedClass,
		InParent,
		InName,
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass(),
		FName("BlueprintTriggerActionFactory")
	);
}
