// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TriggerAction.h"
#include "BlueprintTriggerAction.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class WORLDTRIGGERSYSTEM_API UBlueprintTriggerAction : public UTriggerAction
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent, Category="Trigger", meta=(DisplayName="Execute"))
	void BlueprintExecute(AActor* Context);

	UFUNCTION(BlueprintImplementableEvent, Category="Trigger", meta=(DisplayName="GetEditorValidationWarning"))
	FString BlueprintGetEditorValidationWarning() const;

	virtual void Execute(AActor* Context) override { BlueprintExecute(Context); }

#if WITH_EDITOR
	virtual FString GetEditorValidationWarning() const override { return BlueprintGetEditorValidationWarning(); }
	virtual bool ImplementsGetWorld() const override { return true; }
#endif

	virtual UWorld* GetWorld() const override
	{
		if (const UObject* Outer = GetOuter())
		{
			return Outer->GetWorld();
		}
		return nullptr;
	}
};
