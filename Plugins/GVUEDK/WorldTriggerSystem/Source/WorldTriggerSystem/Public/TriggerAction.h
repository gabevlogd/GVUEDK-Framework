#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TriggerAction.generated.h"

class AWorldTriggerContainer;

/**
 * Base class for all trigger actions.
 *
 * An Action answers the question: "WHAT happens when the trigger fires?"
 *
 * All actor references inside subclasses should use TSoftObjectPtr<AActor>
 * instead of raw AActor* to survive duplication and level copy/paste safely.
 *
 * Access the owning container via GetContainer() – do NOT cache it manually.
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories,
	   HideCategories=(Object))
class WORLDTRIGGERSYSTEM_API UTriggerAction : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * Execute this action.
	 * @param Context  The Actor that activated the trigger.
	 */
	virtual void Execute(AActor* Context) {}

	/**
	 * Helper: walks the Outer chain to find the owning AWorldTriggerContainer.
	 * Never cache this – always call it on demand.
	 */
	AWorldTriggerContainer* GetContainer() const;

#if WITH_EDITOR
	/** Editor-only validation: override to return a warning string, or "" if ok. */
	virtual FString GetEditorValidationWarning() const { return FString(); }
#endif
};
