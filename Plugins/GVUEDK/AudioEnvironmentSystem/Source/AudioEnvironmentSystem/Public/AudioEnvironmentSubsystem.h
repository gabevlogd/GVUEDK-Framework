#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AudioEnvironmentTypes.h"
#include "AudioEnvironmentSubsystem.generated.h"

class AAudioEnvironmentVolume;
class UAudioEnvironmentEvaluator;
class UAudioEnvironmentPlayerListener;
class IAudioEnvironmentListener;
class IAudioEnvironmentBackend;
class APlayerController;

// ============================================================
//  Delegates
// ============================================================

/**
 * Fired when any listener's resolved environment state changes by a
 * meaningful amount. Use this to react to environment transitions.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnAudioEnvironmentStateChanged,
    const FGuid&,                        ListenerID,
    const FAudioEnvironmentListenerState&, NewState);

/**
 * Fired every evaluation tick for all active listeners.
 * Use this for continuous parameter smoothing or interpolation in backends.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnAudioEnvironmentTick,
    const FGuid&,                        ListenerID,
    const FAudioEnvironmentListenerState&, CurrentState);

// ============================================================
//  UAudioEnvironmentSubsystem
// ============================================================

/**
 * World subsystem that owns and orchestrates the Audio Environment System.
 *
 * ── Lifecycle ──────────────────────────────────────────────────────────────
 *
 * Volumes (AAudioEnvironmentVolume) self-register / unregister via
 * BeginPlay / EndPlay. No manual registration is required.
 *
 * Listeners must be registered explicitly. The simplest path:
 *
 *   // In your PlayerController or GameMode BeginPlay:
 *   auto* AES = GetWorld()->GetSubsystem<UAudioEnvironmentSubsystem>();
 *   FGuid MyListenerID = AES->RegisterPlayerListener(this);
 *
 * For custom listeners (split-screen, audio camera, etc.) implement
 * IAudioEnvironmentListener and call:
 *
 *   FGuid ID = AES->RegisterListener(MyListenerObject);
 *
 * ── Evaluation ─────────────────────────────────────────────────────────────
 *
 * The subsystem ticks at a configurable rate (default 10 Hz).
 * Each tick it:
 *   1. Removes stale (destroyed) volume/listener references.
 *   2. Evaluates each active listener via UAudioEnvironmentEvaluator.
 *   3. Fires OnEnvironmentTick for all listeners.
 *   4. Fires OnEnvironmentStateChanged for listeners whose state changed.
 *   5. Notifies all registered IAudioEnvironmentBackend implementations.
 *
 * Source of truth is always spatial position — no enter/exit event history
 * is maintained. Teleports and respawns are handled correctly without
 * special-casing.
 *
 * ── Backend integration ────────────────────────────────────────────────────
 *
 * Connect an audio middleware by implementing IAudioEnvironmentBackend and
 * calling RegisterBackend(). Your backend will receive:
 *   - OnListenerEnvironmentChanged() — when state changes meaningfully.
 *   - OnListenerEnvironmentTick()    — every evaluation tick.
 *
 * ── Debug ──────────────────────────────────────────────────────────────────
 *
 * Console variable:  AudioEnv.DebugDraw 1
 * Blueprint method:  SetDebugVisualizationEnabled(true)
 * Project Settings:  Audio > Audio Environment System > Debug
 */
UCLASS(BlueprintType)
class AUDIOENVIRONMENTSYSTEM_API UAudioEnvironmentSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    // --------------------------------------------------------
    //  UWorldSubsystem / UTickableWorldSubsystem
    // --------------------------------------------------------

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    virtual void     Tick(float DeltaTime) override;
    virtual TStatId  GetStatId() const override;

    // --------------------------------------------------------
    //  Volume registration (called automatically by volumes)
    // --------------------------------------------------------

    /**
     * Registers a volume for evaluation.
     * Called automatically from AAudioEnvironmentVolume::BeginPlay().
     * Safe to call manually for dynamically spawned volumes.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment",
        meta = (ToolTip = "Register a volume for environment evaluation. Called automatically by AAudioEnvironmentVolume::BeginPlay."))
    void RegisterVolume(AAudioEnvironmentVolume* InVolume);

    /**
     * Unregisters a volume.
     * Called automatically from AAudioEnvironmentVolume::EndPlay().
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    void UnregisterVolume(AAudioEnvironmentVolume* InVolume);

    /** Returns the number of currently registered volumes. */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    int32 GetRegisteredVolumeCount() const { return RegisteredVolumes.Num(); }

    // --------------------------------------------------------
    //  Listener registration
    // --------------------------------------------------------

    /**
     * Register any UObject that implements IAudioEnvironmentListener.
     * Returns the stable ListenerID assigned to this listener.
     * Returns an invalid FGuid if InListenerObject is null or does not implement the interface.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment",
        meta = (DisplayName = "Register Listener"))
    FGuid RegisterListener(TScriptInterface<IAudioEnvironmentListener> InListener);

    /**
     * Register a PlayerController's pawn as a listener.
     * Internally creates a UAudioEnvironmentPlayerListener wrapper.
     * Returns the stable ListenerID.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment",
        meta = (ToolTip = "Convenience: register the player's pawn location as a listener."))
    FGuid RegisterPlayerListener(APlayerController* InController);

    /**
     * Unregister a listener by ID.
     * The corresponding state is removed immediately.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    void UnregisterListener(const FGuid& InListenerID);

    /** Returns the number of currently registered listeners. */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    int32 GetRegisteredListenerCount() const { return RegisteredListeners.Num(); }

    // --------------------------------------------------------
    //  Backend registration
    // --------------------------------------------------------

    /**
     * Register an audio backend to receive environment state notifications.
     * The backend object must implement IAudioEnvironmentBackend.
     * Backend::Initialize() is called immediately.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    bool RegisterBackend(UObject* InBackendObject);

    /**
     * Unregister a backend.
     * Backend::Shutdown() is called immediately.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    void UnregisterBackend(UObject* InBackendObject);

    // --------------------------------------------------------
    //  Query API
    // --------------------------------------------------------

    /**
     * Returns the latest resolved state for a given listener.
     * Returns false and leaves OutState unchanged if the listener is not registered.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    bool GetListenerState(const FGuid& InListenerID, FAudioEnvironmentListenerState& OutState) const;

    /**
     * Returns the latest state for the first registered listener.
     * Convenience for single-player games.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    bool GetPrimaryListenerState(FAudioEnvironmentListenerState& OutState) const;

    /**
     * Returns the active volume weights for a specific listener.
     * Useful for custom HUD or debug display.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    TArray<FAudioEnvironmentVolumeWeight> GetActiveVolumesForListener(const FGuid& InListenerID) const;

    /**
     * Forces an immediate re-evaluation of all listeners, bypassing the interval timer.
     * Useful after teleporting a listener or loading a new area.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment",
        meta = (ToolTip = "Force immediate re-evaluation. Useful after teleports or level loads."))
    void ForceEvaluate();

    // --------------------------------------------------------
    //  Delegates
    // --------------------------------------------------------

    /** Fires when any listener's environment state changes by a meaningful amount. */
    UPROPERTY(BlueprintAssignable, Category = "Audio|Environment")
    FOnAudioEnvironmentStateChanged OnEnvironmentStateChanged;

    /** Fires every evaluation tick for all active listeners. */
    UPROPERTY(BlueprintAssignable, Category = "Audio|Environment")
    FOnAudioEnvironmentTick OnEnvironmentTick;

    // --------------------------------------------------------
    //  Debug
    // --------------------------------------------------------

    /** Enable or disable debug visualization at runtime. */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment|Debug")
    void SetDebugVisualizationEnabled(bool bInEnabled);

    /** Returns whether debug visualization is currently enabled. */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment|Debug")
    bool IsDebugVisualizationEnabled() const { return bDebugVisualizationEnabled; }

private:
    // --------------------------------------------------------
    //  Internal data structures
    // --------------------------------------------------------

    /**
     * Internal representation of a registered listener.
     * Non-USTRUCT since it is not exposed to Blueprint.
     */
    struct FListenerEntry
    {
        FGuid                    ListenerID;
        TWeakObjectPtr<UObject>  WeakObject;    // Lifetime sentinel.
        FAudioEnvironmentListenerState LastState;

        /** Returns the interface pointer if the object is still alive. */
        IAudioEnvironmentListener* GetInterface() const;

        bool IsAlive() const { return WeakObject.IsValid(); }
    };

    // --------------------------------------------------------
    //  State
    // --------------------------------------------------------

    UPROPERTY()
    TObjectPtr<UAudioEnvironmentEvaluator> Evaluator;

    /**
     * Player listener wrapper objects created by RegisterPlayerListener().
     * Stored as UPROPERTY so the GC keeps them alive.
     */
    UPROPERTY()
    TArray<TObjectPtr<UAudioEnvironmentPlayerListener>> OwnedPlayerListeners;

    /** All registered environment volumes (weak to handle actor destruction). */
    TArray<TWeakObjectPtr<AAudioEnvironmentVolume>> RegisteredVolumes;

    /** All registered listeners. */
    TArray<FListenerEntry> RegisteredListeners;

    /** All registered backend integrations (weak to handle GC). */
    TArray<TWeakObjectPtr<UObject>> RegisteredBackends;

    /** Time accumulator for evaluation interval throttling. */
    float TimeSinceLastEvaluation = 0.f;

    bool bDebugVisualizationEnabled = false;

    // --------------------------------------------------------
    //  Internal methods
    // --------------------------------------------------------

    void EvaluateAllListeners();
    void EvaluateListener(FListenerEntry& InOutEntry);

    void NotifyStateChanged(
        const FAudioEnvironmentListenerState& InPrev,
        const FAudioEnvironmentListenerState& InNew);

    void NotifyStateTick(const FAudioEnvironmentListenerState& InState);

    void DrawDebug() const;

    void CleanupStaleVolumes();
    void CleanupStaleListeners();
    void CleanupStaleBackends();
};
