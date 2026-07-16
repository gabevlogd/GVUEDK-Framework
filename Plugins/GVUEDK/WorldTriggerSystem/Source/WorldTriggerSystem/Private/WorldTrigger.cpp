#include "WorldTrigger.h"
#include "Engine/World.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogWorldTrigger, Log, All);

// ─── Public API ──────────────────────────────────────────────────────────────

void UWorldTrigger::TryExecuteEnter(AActor* Context)
{
	if (!EvaluateConditions(EnterConditions, EnterConditionLogic, Context, TEXT("Enter")))
	{
		return;
	}

	ScheduleOrExecute(EnterDelay, bEnterRetriggerable,
					  bEnterExecuting, EnterTimerHandle,
					  &UWorldTrigger::ExecuteEnterActions,
					  Context, TEXT("Enter"));
}

void UWorldTrigger::TryExecuteExit(AActor* Context)
{
	if (!EvaluateConditions(ExitConditions, ExitConditionLogic, Context, TEXT("Exit")))
	{
		return;
	}

	ScheduleOrExecute(ExitDelay, bExitRetriggerable,
					  bExitExecuting, ExitTimerHandle,
					  &UWorldTrigger::ExecuteExitActions,
					  Context, TEXT("Exit"));
}

// ─── Condition Evaluation ────────────────────────────────────────────────────

bool UWorldTrigger::EvaluateConditions(const TArray<UTriggerCondition*>& Conditions,
									   EConditionLogicMode LogicMode,
									   const AActor* Context,
									   const FString& EventLabel) const
{
	// No conditions configured → always pass
	if (Conditions.Num() == 0)
	{
		return true;
	}

	// Track which conditions passed so OnTriggered() can be called in one
	// sweep after the overall result is known. This guarantees that state
	// mutation never happens when the evaluation ultimately fails (e.g. OR
	// short-circuit on success mutates only the first passing condition).
	TArray<UTriggerCondition*> PassedConditions;
	PassedConditions.Reserve(Conditions.Num());

	bool bResult = false;

	if (LogicMode == EConditionLogicMode::AND)
	{
		bResult = true;

		for (UTriggerCondition* Cond : Conditions)
		{
			if (!IsValid(Cond))
			{
				UE_LOG(LogWorldTrigger, Warning,
					   TEXT("[%s][%s][AND] Null condition entry – skipping"),
					   *TriggerLabel, *EventLabel);
				continue;
			}

			const bool bPass = Cond->Evaluate(Context);

			if (bDebugLog)
			{
				UE_LOG(LogWorldTrigger, Log, TEXT("[%s][%s][AND] %s → %s"),
					   *TriggerLabel, *EventLabel,
					   *Cond->GetName(), bPass ? TEXT("PASS") : TEXT("FAIL"));
			}

			if (!bPass)
			{
				bResult = false;
				break; // short-circuit: no need to check further
			}

			PassedConditions.Add(Cond);
		}
	}
	else // OR
	{
		bResult = false;

		for (UTriggerCondition* Cond : Conditions)
		{
			if (!IsValid(Cond))
			{
				UE_LOG(LogWorldTrigger, Warning,
					   TEXT("[%s][%s][OR] Null condition entry – skipping"),
					   *TriggerLabel, *EventLabel);
				continue;
			}

			const bool bPass = Cond->Evaluate(Context);

			if (bDebugLog)
			{
				UE_LOG(LogWorldTrigger, Log, TEXT("[%s][%s][OR] %s → %s"),
					   *TriggerLabel, *EventLabel,
					   *Cond->GetName(), bPass ? TEXT("PASS") : TEXT("FAIL"));
			}

			if (bPass)
			{
				PassedConditions.Add(Cond);
				bResult = true;
				break; // short-circuit: one passing condition is enough
			}
		}
	}

	// State mutation phase: only when the overall result is positive
	if (bResult)
	{
		for (UTriggerCondition* Cond : PassedConditions)
		{
			Cond->OnTriggered(Context);
		}
	}

	return bResult;
}

// ─── Scheduling ──────────────────────────────────────────────────────────────

void UWorldTrigger::ScheduleOrExecute(float Delay,
									  bool bRetriggerable,
									  bool& bExecutingFlag,
									  FTimerHandle& TimerHandle,
									  void (UWorldTrigger::*TimerCallback)(TWeakObjectPtr<AActor>),
									  AActor* Context,
									  const FString& EventLabel)
{
	if (bExecutingFlag)
	{
		if (bRetriggerable)
		{
			// Cancel the in-flight timer and restart from scratch
			UWorld* World = GetWorld();
			if (IsValid(World))
			{
				World->GetTimerManager().ClearTimer(TimerHandle);
			}

			if (bDebugLog)
			{
				UE_LOG(LogWorldTrigger, Log,
					   TEXT("[%s][%s] Retriggered – delay timer reset"),
					   *TriggerLabel, *EventLabel);
			}

			// bExecutingFlag stays true; we immediately re-schedule below
		}
		else
		{
			if (bDebugLog)
			{
				UE_LOG(LogWorldTrigger, Log,
					   TEXT("[%s][%s] Blocked – already pending (Retriggerable=false)"),
					   *TriggerLabel, *EventLabel);
			}
			return;
		}
	}

	bExecutingFlag = true;

	if (Delay > 0.f)
	{
		UWorld* World = GetWorld();
		if (!IsValid(World))
		{
			UE_LOG(LogWorldTrigger, Error,
				   TEXT("[%s][%s] Cannot schedule delay: World is invalid"),
				   *TriggerLabel, *EventLabel);
			bExecutingFlag = false;
			return;
		}

		TWeakObjectPtr<AActor> WeakContext(Context);

		World->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateUObject(this, TimerCallback, WeakContext),
			Delay,
			false
		);
		// bExecutingFlag intentionally left true – represents "pending"
	}
	else
	{
		// Fire immediately through the appropriate callback
		(this->*TimerCallback)(TWeakObjectPtr<AActor>(Context));
	}
}

// ─── Timer Callbacks ─────────────────────────────────────────────────────────

void UWorldTrigger::ExecuteEnterActions(TWeakObjectPtr<AActor> WeakContext)
{
	RunActions(EnterActions, bEnterExecuting, WeakContext, TEXT("Enter"));
}

void UWorldTrigger::ExecuteExitActions(TWeakObjectPtr<AActor> WeakContext)
{
	RunActions(ExitActions, bExitExecuting, WeakContext, TEXT("Exit"));
}

// ─── Action Loop ─────────────────────────────────────────────────────────────

void UWorldTrigger::RunActions(TArray<UTriggerAction*>& Actions,
							   bool& bExecutingFlag,
							   TWeakObjectPtr<AActor> WeakContext,
							   const FString& EventLabel)
{
	AActor* Context = WeakContext.Get();

	if (!IsValid(Context))
	{
		UE_LOG(LogWorldTrigger, Warning,
			   TEXT("[%s][%s] Context Actor no longer valid – actions skipped"),
			   *TriggerLabel, *EventLabel);
		bExecutingFlag = false;
		return;
	}

	if (bDebugLog)
	{
		UE_LOG(LogWorldTrigger, Log,
			   TEXT("[%s][%s] Running %d action(s)"),
			   *TriggerLabel, *EventLabel, Actions.Num());
	}

	for (UTriggerAction* Action : Actions)
	{
		if (!IsValid(Action))
		{
			UE_LOG(LogWorldTrigger, Warning,
				   TEXT("[%s][%s] Null action entry – skipping"),
				   *TriggerLabel, *EventLabel);
			continue;
		}

		if (bDebugLog)
		{
			UE_LOG(LogWorldTrigger, Log,
				   TEXT("[%s][%s] → %s"), *TriggerLabel, *EventLabel, *Action->GetName());
		}

		Action->Execute(Context);
	}

	bExecutingFlag = false;
}
