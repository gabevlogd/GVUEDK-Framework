#pragma once

#include "CoreMinimal.h"
#include "AudioEnvironmentTypes.generated.h"

// ============================================================
//  Enumerations
// ============================================================

/**
 * Falloff curve applied when blending the weight of a volume near its boundary.
 */
UENUM(BlueprintType)
enum class EAudioEnvironmentBlendMode : uint8
{
    /** Constant linear ramp from boundary inward. */
    Linear      UMETA(DisplayName = "Linear"),

    /** Smooth S-curve (3t^2 - 2t^3). Preferred for most cases. */
    Smoothstep  UMETA(DisplayName = "Smoothstep"),

    /**
     * Driven by a UCurveFloat asset assigned on the volume.
     * X axis = normalized depth [0..1], Y axis = output weight [0..1].
     * Falls back to Smoothstep if no curve is assigned.
     */
    Curve       UMETA(DisplayName = "Curve"),
};

/**
 * Determines how overlapping volumes of different priorities contribute
 * to the final blended parameter set.
 */
UENUM(BlueprintType)
enum class EAudioEnvironmentPriorityMode : uint8
{
    /**
     * All active volumes contribute proportionally by weight.
     * Volumes are grouped by LayerTag first; layers compete by total weight.
     */
    Additive    UMETA(DisplayName = "Additive"),

    /**
     * Only the highest-priority volume (or group) contributes.
     * Volumes sharing the top priority are blended additively.
     */
    Override    UMETA(DisplayName = "Override"),
};

// ============================================================
//  FAudioEnvironmentParameters
// ============================================================

/**
 * Generic named float parameter map. This is the universal currency used
 * throughout the system to represent environment data.
 *
 * Keys are agreed upon by convention between this system and whatever audio
 * backend is used (FMOD, Wwise, Unreal Audio, custom). No keys are hardcoded
 * here. Typical examples:
 *
 *   ReverbAmount   = 0.8
 *   OcclusionFactor = 0.0
 *   ExteriorFactor  = 1.0
 *   WindIntensity   = 0.3
 *   StormIntensity  = 0.0
 */
USTRUCT(BlueprintType)
struct AUDIOENVIRONMENTSYSTEM_API FAudioEnvironmentParameters
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Environment",
        meta = (ToolTip = "Named float parameters. Keys must match your audio backend's expected parameter names."))
    TMap<FName, float> Values;

    FAudioEnvironmentParameters() = default;

    // --------------------------------------------------------
    //  Mutation helpers
    // --------------------------------------------------------

    /** Set or overwrite a single named parameter. */
    void SetParameter(FName InKey, float InValue);

    /** Get a named parameter value. Returns InDefault if the key is not present. */
    float GetParameter(FName InKey, float InDefault = 0.f) const;

    /** Returns true if the key exists in this set. */
    bool HasParameter(FName InKey) const;

    /** Merge all parameters from InOther, overwriting any keys that already exist. */
    void MergeFrom(const FAudioEnvironmentParameters& InOther);

    /**
     * Merge default values from InDefaults.
     * Only inserts keys that are NOT already present (non-destructive).
     */
    void MergeDefaults(const FAudioEnvironmentParameters& InDefaults);

    // --------------------------------------------------------
    //  Static blending utilities
    // --------------------------------------------------------

    /**
     * Linear interpolation between two parameter sets.
     * The union of both key sets is used; missing keys are treated as 0.
     */
    static FAudioEnvironmentParameters Lerp(
        const FAudioEnvironmentParameters& InA,
        const FAudioEnvironmentParameters& InB,
        float InAlpha);

    // --------------------------------------------------------
    //  Comparison
    // --------------------------------------------------------

    bool operator==(const FAudioEnvironmentParameters& Other) const;
    bool operator!=(const FAudioEnvironmentParameters& Other) const { return !(*this == Other); }
};

// ============================================================
//  FAudioEnvironmentVolumeWeight
// ============================================================

/**
 * The evaluated contribution of a single AAudioEnvironmentVolume to a
 * listener's environment state at a point in time.
 */
USTRUCT(BlueprintType)
struct AUDIOENVIRONMENTSYSTEM_API FAudioEnvironmentVolumeWeight
{
    GENERATED_BODY()

    /** The contributing volume. */
    UPROPERTY(BlueprintReadOnly, Category = "Audio|Environment")
    TObjectPtr<class AAudioEnvironmentVolume> Volume = nullptr;

    /** Normalized blend weight [0..1]. 0 = no contribution. */
    UPROPERTY(BlueprintReadOnly, Category = "Audio|Environment")
    float Weight = 0.f;

    /** Priority at evaluation time (cached from volume for sorting). */
    UPROPERTY(BlueprintReadOnly, Category = "Audio|Environment")
    int32 Priority = 0;

    /** Layer tag at evaluation time (cached from volume). */
    UPROPERTY(BlueprintReadOnly, Category = "Audio|Environment")
    FName LayerTag = NAME_None;

    bool IsValid() const { return Volume != nullptr && Weight > KINDA_SMALL_NUMBER; }
};

// ============================================================
//  FAudioEnvironmentListenerState
// ============================================================

/**
 * Fully resolved audio environment state for a single listener.
 * This is what backend integrations (FMOD, Wwise, etc.) consume.
 */
USTRUCT(BlueprintType)
struct AUDIOENVIRONMENTSYSTEM_API FAudioEnvironmentListenerState
{
    GENERATED_BODY()

    /** Stable GUID identifying the listener across evaluations. */
    UPROPERTY(BlueprintReadOnly, Category = "Audio|Environment")
    FGuid ListenerID;

    /** World-space location used for this evaluation. */
    UPROPERTY(BlueprintReadOnly, Category = "Audio|Environment")
    FVector Location = FVector::ZeroVector;

    /** Final weighted blend of all contributing volumes' parameters. */
    UPROPERTY(BlueprintReadOnly, Category = "Audio|Environment")
    FAudioEnvironmentParameters BlendedParameters;

    /**
     * All volumes that produced a non-zero weight this evaluation.
     * Sorted by descending weight.
     */
    UPROPERTY(BlueprintReadOnly, Category = "Audio|Environment")
    TArray<FAudioEnvironmentVolumeWeight> ActiveVolumes;

    /** World time (seconds) at which this state was computed. */
    UPROPERTY(BlueprintReadOnly, Category = "Audio|Environment")
    float EvaluationTime = -1.f;

    bool IsValid() const { return ListenerID.IsValid(); }
};
