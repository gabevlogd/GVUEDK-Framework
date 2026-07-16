#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldTrigger.h"
#include "WorldTriggerContainer.generated.h"

/**
 * Scene Actor that holds an array of WorldTriggers.
 *
 * Place this in a level, then populate the Triggers array via the Details panel.
 * Each trigger exposes independent Enter and Exit events with their own
 * conditions, actions, delay, and retriggerable flag.
 *
 * The container binds to both OnComponentBeginOverlap and OnComponentEndOverlap
 * and forwards the Context Actor to TryExecuteEnter / TryExecuteExit on each trigger.
 *
 * Triggers is marked EditInstanceOnly: configure per-instance in the Details panel,
 * not in the Blueprint Class Defaults, to avoid override confusion after recompiles.
 *
 * Multiplayer: TryExecuteAll / TryExecuteExitAll run only on the server.
 * Individual Actions are responsible for any client-side RPC / cosmetics.
 */
UCLASS(HideCategories=(Rendering, Replication, Input, Actor, LOD, Cooking))
class WORLDTRIGGERSYSTEM_API AWorldTriggerContainer : public AActor
{
	GENERATED_BODY()

public:

	AWorldTriggerContainer();

protected:

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
#endif

public:

	// ─── Components ──────────────────────────────────────────────────────────

	/** Collision volume that detects overlapping Actors. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WorldTrigger|Components")
	class UBoxComponent* TriggerVolume;

#if WITH_EDITORONLY_DATA
	/** Optional text label displayed above the trigger volume. */
	UPROPERTY()
	class UTextRenderComponent* ContainerLabelComponent;
#endif

	// ─── Trigger List ────────────────────────────────────────────────────────

	/**
	 * The triggers configured for this container.
	 * EditInstanceOnly: set per-instance in Details, not in the Blueprint archetype.
	 */
	UPROPERTY(EditInstanceOnly, Instanced, Category="WorldTrigger", meta=(DisplayName="Triggers"))
	TArray<UWorldTrigger*> Triggers;

	// ─── Settings ────────────────────────────────────────────────────────────

	/** Master switch. When false, all overlap events are silently ignored. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WorldTrigger|Settings")
	bool bEnabled = true;

	/**
	 * If true, disables the container after the first Enter cycle where
	 * at least one trigger reaches its action execution phase.
	 */
	UPROPERTY(EditAnywhere, Category="WorldTrigger|Settings")
	bool bDisableAfterFirstExecution = false;

#if WITH_EDITORONLY_DATA
	/** Optional text label displayed above the trigger volume. */
	UPROPERTY(EditAnywhere, Category="WorldTrigger|Settings")
	FText ContainerLabel;

	/** Debug visualization color for the trigger volume. */
	UPROPERTY(EditAnywhere, Category="WorldTrigger|Settings")
	FColor TriggerBoxColor = FColor::Red;

	/**
	 * If true, the container is hidden in-game but visible in the editor.
	 * This only affects the visibility of the Actor and its components –
	 * collision and trigger functionality remain active regardless.
	 */
	UPROPERTY(EditAnywhere, Category="WorldTrigger|Settings")
	bool bHiddenInGame = true;
#endif

	// ─── Runtime API ─────────────────────────────────────────────────────────

	/**
	 * Manually trigger all Enter events with the given Context.
	 * Useful for calling from Blueprint without relying on overlap.
	 */
	UFUNCTION(BlueprintCallable, Category="WorldTrigger")
	void TryExecuteEnterAll(AActor* Context);

	/**
	 * Manually trigger all Exit events with the given Context.
	 * Useful for calling from Blueprint without relying on overlap.
	 */
	UFUNCTION(BlueprintCallable, Category="WorldTrigger")
	void TryExecuteExitAll(AActor* Context);

	UFUNCTION(BlueprintCallable, Category="WorldTrigger")
	void SetEnabled(bool bNewEnabled) { bEnabled = bNewEnabled; }

private:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
					  AActor* OtherActor,
					  UPrimitiveComponent* OtherComp,
					  int32 OtherBodyIndex);

	bool bHasExecutedOnce = false;
};
