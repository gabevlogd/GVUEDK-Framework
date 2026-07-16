#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AudioEnvironmentTypes.h"
#include "IAudioEnvironmentBackend.generated.h"

UINTERFACE(MinimalAPI, BlueprintType,
    meta = (ToolTip = "Implement this interface to connect the Audio Environment System to an audio middleware (FMOD, Wwise, Unreal Audio, etc.)."))
class UAudioEnvironmentBackend : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for audio middleware backends.
 *
 * Implement this interface to connect the AudioEnvironmentSystem to a specific
 * audio engine without modifying any core plugin code.
 *
 * Example implementations:
 *   - UFMODAudioEnvironmentBackend   (reads BlendedParameters, sets FMOD globals)
 *   - UWwiseAudioEnvironmentBackend  (sets RTPC values)
 *   - UUnrealAudioEnvironmentBackend (drives SubmixEffect parameters)
 *
 * Registration:
 *   UAudioEnvironmentSubsystem::RegisterBackend(MyBackend);
 *
 * A single backend instance can handle multiple listeners by inspecting
 * the ListenerID parameter.
 */
class AUDIOENVIRONMENTSYSTEM_API IAudioEnvironmentBackend
{
    GENERATED_BODY()

public:
    /**
     * Called when a listener's blended environment state has changed by a
     * meaningful amount since the last evaluation.
     *
     * This is where you apply BlendedParameters to your audio engine.
     * Read NewState.BlendedParameters.Values for the current named float set.
     *
     * @param InListenerID   The listener whose state changed.
     * @param InPrevState    The previous resolved state.
     * @param InNewState     The new resolved state.
     */
    virtual void OnListenerEnvironmentChanged(
        const FGuid& InListenerID,
        const FAudioEnvironmentListenerState& InPrevState,
        const FAudioEnvironmentListenerState& InNewState) = 0;

    /**
     * Called every evaluation tick regardless of whether the state changed.
     * Use this for continuous parameter smoothing, interpolation, or logging.
     * Default implementation does nothing.
     *
     * @param InListenerID   The listener being ticked.
     * @param InCurrentState The latest resolved state.
     */
    virtual void OnListenerEnvironmentTick(
        const FGuid& InListenerID,
        const FAudioEnvironmentListenerState& InCurrentState) {}

    /**
     * Called once when the backend is registered with the subsystem.
     * Use this to initialize any middleware-side state.
     */
    virtual void Initialize() {}

    /**
     * Called when the backend is unregistered or the subsystem shuts down.
     * Use this to clean up middleware-side state.
     */
    virtual void Shutdown() {}
};
