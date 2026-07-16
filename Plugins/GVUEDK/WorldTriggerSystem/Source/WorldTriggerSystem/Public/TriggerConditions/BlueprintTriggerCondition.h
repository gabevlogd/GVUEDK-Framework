// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TriggerCondition.h"
#include "BlueprintTriggerCondition.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class WORLDTRIGGERSYSTEM_API UBlueprintTriggerCondition : public UTriggerCondition
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent, Category="Trigger", meta=(DisplayName="Evaluate"))
	bool BlueprintEvaluate(const AActor* Context) const;

	UFUNCTION(BlueprintImplementableEvent, Category="Trigger", meta=(DisplayName="OnTriggered"))
	void BlueprintOnTriggered(const AActor* Context);

	UFUNCTION(BlueprintImplementableEvent, Category="Trigger", meta=(DisplayName="GetEditorValidationWarning"))
	FString BlueprintGetEditorValidationWarning() const;

	virtual bool Evaluate(const AActor* Context) const override
	{
		return bNegateConditionResult ? !BlueprintEvaluate(Context) : BlueprintEvaluate(Context);
	}

	virtual void OnTriggered(const AActor* Context) override { BlueprintOnTriggered(Context); }

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

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ToolTip = "If true, the condition result will be negated."))
	bool bNegateConditionResult;
};
