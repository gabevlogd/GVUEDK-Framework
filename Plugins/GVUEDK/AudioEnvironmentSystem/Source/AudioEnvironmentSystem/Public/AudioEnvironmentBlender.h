#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AudioEnvironmentTypes.h"
#include "AudioEnvironmentBlender.generated.h"

/**
 * Stateless utility library for blending audio environment parameters.
 *
 * All methods are static and pure. This class holds no state.
 * It is a BlueprintFunctionLibrary so its utilities are accessible from Blueprint.
 *
 * The blend pipeline (called by UAudioEnvironmentEvaluator):
 *
 *   1. GatherVolumeWeights()       — evaluates raw spatial weights per volume
 *   2. Blend() / AdditiveBlend()   — resolves the final parameter set
 *   3. Backend receives result      — e.g. FMOD global parameter update
 *
 * Layer-aware blending (Additive mode):
 *   Volumes are grouped by LayerTag. Within each layer, weights are normalized
 *   and blended additively. The resulting per-layer parameter sets are then
 *   blended together using the layers' combined weights. This models independent
 *   acoustic dimensions (e.g. "Weather" and "Room") that compose cleanly.
 */
UCLASS(meta = (DisplayName = "Audio Environment Blender"))
class AUDIOENVIRONMENTSYSTEM_API UAudioEnvironmentBlender : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // --------------------------------------------------------
    //  Primary blend entry point
    // --------------------------------------------------------

    /**
     * Dispatches to AdditiveBlend or OverrideBlend based on InPriorityMode.
     * This is the main function called by the evaluator.
     *
     * @param InVolumeWeights  Evaluated volume weights (output of GatherVolumeWeights).
     * @param InPriorityMode   How volumes of different priorities interact.
     * @return                 Final blended parameter set ready for backend consumption.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment|Blending")
    static FAudioEnvironmentParameters Blend(
        const TArray<FAudioEnvironmentVolumeWeight>& InVolumeWeights,
        EAudioEnvironmentPriorityMode InPriorityMode);

    // --------------------------------------------------------
    //  Blend modes
    // --------------------------------------------------------

    /**
     * Layer-aware additive blend.
     *
     * Algorithm:
     *   1. Group volumes by LayerTag.
     *   2. Within each layer: normalize weights, compute weighted parameter sum.
     *   3. Blend layers together using each layer's total (unnormalized) weight.
     *
     * All volumes contribute proportionally; no priority-based exclusion.
     *
     * @param InVolumeWeights  Volumes with non-zero weights.
     * @return                 Layer-blended parameter set.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment|Blending")
    static FAudioEnvironmentParameters AdditiveBlend(
        const TArray<FAudioEnvironmentVolumeWeight>& InVolumeWeights);

    /**
     * Priority-based override blend.
     *
     * Only the highest-priority volume group contributes.
     * Volumes sharing the top priority are blended additively amongst themselves.
     * Lower-priority volumes are completely ignored.
     *
     * @param InVolumeWeights  Volumes with non-zero weights.
     * @return                 Override-resolved parameter set.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment|Blending")
    static FAudioEnvironmentParameters OverrideBlend(
        const TArray<FAudioEnvironmentVolumeWeight>& InVolumeWeights);

    // --------------------------------------------------------
    //  Curve utilities
    // --------------------------------------------------------

    /**
     * Smoothstep function: f(t) = 3t² - 2t³
     * Input is NOT clamped. Clamp before calling if needed.
     * Result is in [0..1] for input in [0..1].
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment|Blending",
        meta = (ToolTip = "Smooth S-curve: 3t^2 - 2t^3. Best for perceptually smooth volume crossfades."))
    static float Smoothstep(float InT);

    /**
     * Applies a blend mode curve to a normalized depth value.
     * NOTE: For EAudioEnvironmentBlendMode::Curve, this returns the Smoothstep
     * fallback since the curve asset is not available in a static context.
     * Use AAudioEnvironmentVolume::ApplyBlendCurve() when the asset is accessible.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment|Blending")
    static float ApplyBlendMode(float InNormalizedDepth, EAudioEnvironmentBlendMode InMode);

private:
    /**
     * Inner weighted sum: given a set of (params, normalizedWeight) pairs
     * where weights already sum to 1, returns their weighted parameter blend.
     */
    struct FWeightedParamEntry
    {
        FAudioEnvironmentParameters Params;
        float Weight = 0.f;
    };

    static FAudioEnvironmentParameters WeightedSum(const TArray<FWeightedParamEntry>& InEntries);
};
