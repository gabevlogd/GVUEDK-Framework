#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TriggerCondition.generated.h"

/**
 * Base class for all trigger conditions.
 *
 * A Condition answers the question: "CAN this trigger fire right now?"
 * 
 * Subclasses implement Evaluate() as a pure read-only check.
 * Side-effects (e.g. updating cooldown state) go in OnTriggered(),
 * which is called by the system only after ALL conditions pass.
 *
 * Design rules:
 *   - Evaluate()    → pure, no side-effects
 *   - OnTriggered() → state mutation (cooldowns, counters, etc.)
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories,
	   HideCategories=(Object))
class WORLDTRIGGERSYSTEM_API UTriggerCondition : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * Returns true if this condition is currently satisfied.
	 * Must be pure – no state mutation here.
	 * @param Context  The Actor that entered/activated the trigger zone.
	 */
	virtual bool Evaluate(const AActor* Context) const { return true; }

	/**
	 * Called once after all conditions have passed and the trigger is
	 * about to execute. Use this for state mutation (cooldown timers, etc.)
	 * @param Context  The Actor that activated the trigger.
	 */
	virtual void OnTriggered(const AActor* Context) {}

#if WITH_EDITOR
	/** Editor-only validation: override to return a warning string, or "" if ok. */
	virtual FString GetEditorValidationWarning() const { return FString(); }
#endif
};
