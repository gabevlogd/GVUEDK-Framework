#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AudioEnvironmentTypes.h"
#include "AudioEnvironmentEvaluator.generated.h"

class AAudioEnvironmentVolume;

/**
 * Stateless evaluator that computes a listener's full environment state
 * from a set of registered volumes and a world-space location.
 *
 * Architecture notes:
 * ───────────────────
 * - The evaluator holds NO per-listener state; all inputs and outputs are
 *   passed by value. This makes it trivially safe to call from multiple
 *   contexts and easy to unit-test in isolation.
 *
 * - The evaluation pipeline is:
 *     1. Proximity cull: sort volumes by bounding sphere distance, keep nearest N.
 *     2. Weight evaluation: call EvaluateWeightAtLocation() on each candidate.
 *     3. Discard zero-weight volumes.
 *     4. Sort active volumes by descending weight (for debug readability).
 *     5. Delegate parameter blending to UAudioEnvironmentBlender.
 *
 * - Source of truth is always spatial position.
 *   No history, no enter/exit events. Calling Evaluate() with the same
 *   location always produces the same result, regardless of how the
 *   listener arrived there.
 *
 * Future work:
 *   - EvaluateAsync() variant that can be offloaded to a TaskGraph thread.
 *   - Per-listener cache for skipping evaluation when location hasn't moved.
 */
UCLASS(meta = (DisplayName = "Audio Environment Evaluator"))
class AUDIOENVIRONMENTSYSTEM_API UAudioEnvironmentEvaluator : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Evaluates the full audio environment state for a single listener.
     *
     * @param InListenerID        Stable GUID identifying the listener.
     * @param InListenerLocation  World-space position to evaluate from.
     * @param InVolumes           All registered, enabled volumes.
     * @param InMaxVolumes        Evaluation budget (volumes sorted by proximity first).
     * @param InPriorityMode      How volumes of different priorities combine.
     * @param InCurrentTime       World time in seconds (for state timestamping).
     * @return                    Fully resolved FAudioEnvironmentListenerState.
     */
    FAudioEnvironmentListenerState Evaluate(
        const FGuid&                                        InListenerID,
        const FVector&                                      InListenerLocation,
        const TArray<TWeakObjectPtr<AAudioEnvironmentVolume>>& InVolumes,
        int32                                               InMaxVolumes,
        EAudioEnvironmentPriorityMode                       InPriorityMode,
        float                                               InCurrentTime) const;

    /**
     * Returns true if the difference between two states is significant enough
     * to warrant a change notification.
     *
     * Checks both the active volume set and each parameter's value.
     *
     * @param InThreshold  Minimum absolute parameter delta considered meaningful.
     */
    bool HasStateChangedSignificantly(
        const FAudioEnvironmentListenerState& InPrev,
        const FAudioEnvironmentListenerState& InNext,
        float InThreshold = 0.001f) const;

private:
    /**
     * Builds the array of FAudioEnvironmentVolumeWeight for a given location.
     *
     * Volumes are first sorted by bounding sphere proximity and clipped to
     * InMaxVolumes. Only volumes that produce weight > KINDA_SMALL_NUMBER
     * are included in the result.
     *
     * Results are sorted by descending weight for readable debug output.
     */
    TArray<FAudioEnvironmentVolumeWeight> GatherVolumeWeights(
        const FVector&                                         InLocation,
        const TArray<TWeakObjectPtr<AAudioEnvironmentVolume>>& InVolumes,
        int32                                                  InMaxVolumes) const;
};
