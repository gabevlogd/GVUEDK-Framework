#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TriggerCondition.h"
#include "TriggerAction.h"
#include "WorldTrigger.generated.h"

// ─── Condition Logic Mode ────────────────────────────────────────────────────

UENUM(BlueprintType)
enum class EConditionLogicMode : uint8
{
	/** ALL conditions must pass. Short-circuits on first failure. */
	AND UMETA(DisplayName="AND  (all must pass)"),

	/** AT LEAST ONE condition must pass. Short-circuits on first success. */
	OR  UMETA(DisplayName="OR   (any must pass)"),
};

// ─── WorldTrigger ────────────────────────────────────────────────────────────

/**
 * A single trigger unit with independent Enter and Exit event handling.
 *
 * ENTER fires when an Actor overlaps the container volume.
 * EXIT  fires when that Actor leaves the container volume.
 *
 * Each event has its own:
 *   - Conditions array  (with AND / OR evaluation logic)
 *   - Actions array     (executed in order)
 *   - Delay             (seconds before actions run)
 *   - Retriggerable     (resets the pending timer on a new activation)
 *
 * Execution contract (identical for both events):
 *   1. Evaluate Conditions with the chosen LogicMode.
 *   2. If passed → call OnTriggered() on each passing Condition.
 *   3. Wait Delay seconds (optional).
 *   4. Verify Context is still alive.
 *   5. Execute Actions in array order.
 *
 * Enter and Exit guards/timers are fully independent:
 * both can be pending simultaneously without conflict.
 */
UCLASS(EditInlineNew, DefaultToInstanced, HideCategories=(Object))
class WORLDTRIGGERSYSTEM_API UWorldTrigger : public UObject
{
	GENERATED_BODY()

public:

	// ─── Identity ────────────────────────────────────────────────────────────

	/** Optional label for quick identification in the Details panel. */
	UPROPERTY(EditAnywhere, Category="Trigger", meta=(DisplayName="Label"))
	FString TriggerLabel = TEXT("Trigger");

	/** Prints condition results and action names to the Output Log. */
	UPROPERTY(EditAnywhere, Category="Trigger", meta=(DisplayName="Debug Log"))
	bool bDebugLog = false;

	// ─── Enter Event ─────────────────────────────────────────────────────────

	/** How Enter conditions are combined. */
	UPROPERTY(EditAnywhere, Category="Trigger|Enter", meta=(DisplayName="Condition Logic"))
	EConditionLogicMode EnterConditionLogic = EConditionLogicMode::AND;

	/** Conditions evaluated when an Actor enters the volume. */
	UPROPERTY(EditAnywhere, Instanced, Category="Trigger|Enter", meta=(DisplayName="Conditions"))
	TArray<UTriggerCondition*> EnterConditions;

	/** Actions executed when Enter conditions pass. */
	UPROPERTY(EditAnywhere, Instanced, Category="Trigger|Enter", meta=(DisplayName="Actions"))
	TArray<UTriggerAction*> EnterActions;

	/** Seconds to wait before executing Enter actions. */
	UPROPERTY(EditAnywhere, Category="Trigger|Enter", meta=(DisplayName="Delay", ClampMin="0.0", UIMin="0.0"))
	float EnterDelay = 0.f;

	/**
	 * If true, a new Enter activation resets the pending delay timer.
	 * If false, new activations are ignored while a delayed execution is pending.
	 */
	UPROPERTY(EditAnywhere, Category="Trigger|Enter", meta=(DisplayName="Retriggerable"))
	bool bEnterRetriggerable = false;

	// ─── Exit Event ──────────────────────────────────────────────────────────

	/** How Exit conditions are combined. */
	UPROPERTY(EditAnywhere, Category="Trigger|Exit", meta=(DisplayName="Condition Logic"))
	EConditionLogicMode ExitConditionLogic = EConditionLogicMode::AND;

	/** Conditions evaluated when an Actor leaves the volume. */
	UPROPERTY(EditAnywhere, Instanced, Category="Trigger|Exit", meta=(DisplayName="Conditions"))
	TArray<UTriggerCondition*> ExitConditions;

	/** Actions executed when Exit conditions pass. */
	UPROPERTY(EditAnywhere, Instanced, Category="Trigger|Exit", meta=(DisplayName="Actions"))
	TArray<UTriggerAction*> ExitActions;

	/** Seconds to wait before executing Exit actions. */
	UPROPERTY(EditAnywhere, Category="Trigger|Exit", meta=(DisplayName="Delay", ClampMin="0.0", UIMin="0.0"))
	float ExitDelay = 0.f;

	/**
	 * If true, a new Exit activation resets the pending delay timer.
	 * If false, new activations are ignored while a delayed execution is pending.
	 */
	UPROPERTY(EditAnywhere, Category="Trigger|Exit", meta=(DisplayName="Retriggerable"))
	bool bExitRetriggerable = false;

	// ─── Runtime API ─────────────────────────────────────────────────────────

	/** Called by the container when an Actor enters the volume. */
	void TryExecuteEnter(AActor* Context);

	/** Called by the container when an Actor leaves the volume. */
	void TryExecuteExit(AActor* Context);

	bool IsEnterPending() const { return bEnterExecuting; }
	bool IsExitPending()  const { return bExitExecuting;  }

private:

	// ─── Guards & timers ─────────────────────────────────────────────────────
	// Enter and Exit state is fully separate to allow simultaneous pending.

	bool bEnterExecuting = false;
	bool bExitExecuting  = false;

	FTimerHandle EnterTimerHandle;
	FTimerHandle ExitTimerHandle;

	// ─── Helpers ─────────────────────────────────────────────────────────────

	/**
	 * Evaluates a condition array using AND or OR logic.
	 * OnTriggered() is called on passing conditions ONLY when the overall
	 * result is true, so state mutation never occurs on a failed evaluation.
	 */
	bool EvaluateConditions(const TArray<UTriggerCondition*>& Conditions,
							EConditionLogicMode LogicMode,
							const AActor* Context,
							const FString& EventLabel) const;

	/**
	 * Applies the retriggerable/guard logic, then fires immediately
	 * or schedules the appropriate timer callback.
	 */
	void ScheduleOrExecute(float Delay,
						   bool bRetriggerable,
						   bool& bExecutingFlag,
						   FTimerHandle& TimerHandle,
						   void (UWorldTrigger::*TimerCallback)(TWeakObjectPtr<AActor>),
						   AActor* Context,
						   const FString& EventLabel);

	// Timer callbacks – each accesses its own member action array directly,
	// avoiding any reference-through-delegate issues.
	void ExecuteEnterActions(TWeakObjectPtr<AActor> WeakContext);
	void ExecuteExitActions(TWeakObjectPtr<AActor> WeakContext);

	/** Shared action-loop logic called by both timer callbacks. */
	void RunActions(TArray<UTriggerAction*>& Actions,
					bool& bExecutingFlag,
					TWeakObjectPtr<AActor> WeakContext,
					const FString& EventLabel);
};
