#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AudioEnvironmentTypes.h"
#include "AudioEnvironmentSettings.generated.h"

/**
 * Project-level configuration for the Audio Environment System.
 *
 * Accessible via:
 *   Project Settings > Audio > Audio Environment System
 *
 * All settings are saved to DefaultGame.ini and can be overridden
 * per-platform via platform-specific .ini files.
 */
UCLASS(Config = Game, DefaultConfig,
    meta = (DisplayName = "Audio Environment System"))
class AUDIOENVIRONMENTSYSTEM_API UAudioEnvironmentSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UAudioEnvironmentSettings();

    // --------------------------------------------------------
    //  Evaluation
    // --------------------------------------------------------

    /**
     * Evaluation interval in seconds.
     * The subsystem re-evaluates all listener states at this frequency.
     *
     * Lower values = more responsive transitions, higher CPU cost.
     * Recommended range: 0.05s (20 Hz) to 0.2s (5 Hz).
     * Default: 0.1s (10 Hz).
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Evaluation",
        meta = (ClampMin = "0.016", ClampMax = "2.0", UIMin = "0.016", UIMax = "1.0",
            ToolTip = "How often (in seconds) the subsystem evaluates environment states. Lower = more responsive, higher CPU cost."))
    float EvaluationInterval = 0.1f;

    /**
     * Maximum number of volumes evaluated per listener per tick.
     * Volumes are sorted by proximity (nearest bounding sphere first)
     * before the limit is applied, ensuring the most relevant ones
     * are always considered.
     *
     * Increase for large dense scenes; decrease for performance-constrained platforms.
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Evaluation",
        meta = (ClampMin = "1", ClampMax = "64",
            ToolTip = "Max volumes evaluated per listener per tick. Nearest volumes are prioritized."))
    int32 MaxVolumesPerListener = 16;

    /**
     * Parameter change threshold for state-changed notifications.
     * If the maximum absolute difference between any parameter in the
     * previous and new state is below this value, no change event is fired.
     * Prevents spurious callbacks from floating-point noise.
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Evaluation",
        meta = (ClampMin = "0.0", ClampMax = "0.1",
            ToolTip = "Minimum parameter delta to trigger a state-changed notification. Prevents noise-driven callbacks."))
    float ChangeNotificationThreshold = 0.001f;

    // --------------------------------------------------------
    //  Blending
    // --------------------------------------------------------

    /**
     * Default blend mode applied when a volume does not specify one explicitly.
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blending",
        meta = (ToolTip = "Default falloff curve type for volumes that do not override it."))
    EAudioEnvironmentBlendMode DefaultBlendMode = EAudioEnvironmentBlendMode::Smoothstep;

    /**
     * Default priority mode applied when resolving overlapping volumes.
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blending",
        meta = (ToolTip = "Default priority resolution strategy for overlapping volumes."))
    EAudioEnvironmentPriorityMode DefaultPriorityMode = EAudioEnvironmentPriorityMode::Additive;

    // --------------------------------------------------------
    //  Debug
    // --------------------------------------------------------

    /**
     * Enable persistent debug visualization.
     * Also controllable at runtime via the console variable: AudioEnv.DebugDraw 1
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Debug",
        meta = (ToolTip = "Draw debug shapes and text for active volumes and listener states. Also toggled via AudioEnv.DebugDraw."))
    bool bEnableDebugVisualization = false;

    /**
     * Duration in seconds that debug shapes remain visible.
     * 0 = drawn for a single frame only (cheapest).
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Debug",
        meta = (ClampMin = "0.0", ClampMax = "5.0", EditCondition = "bEnableDebugVisualization",
            ToolTip = "How many seconds debug shapes remain visible. 0 = single frame."))
    float DebugDrawDuration = 0.f;

    /** Color for volumes actively contributing to any listener's state. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Debug",
        meta = (EditCondition = "bEnableDebugVisualization"))
    FLinearColor DebugActiveVolumeColor = FLinearColor(0.f, 1.f, 0.2f, 0.6f);

    /** Color for registered volumes that are in range but contributing zero weight. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Debug",
        meta = (EditCondition = "bEnableDebugVisualization"))
    FLinearColor DebugInactiveVolumeColor = FLinearColor(1.f, 0.5f, 0.f, 0.25f);

    /** Color for listener position markers. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Debug",
        meta = (EditCondition = "bEnableDebugVisualization"))
    FLinearColor DebugListenerColor = FLinearColor(1.f, 1.f, 0.f, 1.f);

    // --------------------------------------------------------
    //  UDeveloperSettings
    // --------------------------------------------------------

    virtual FName GetCategoryName() const override { return TEXT("Audio"); }
    virtual FName GetSectionName()  const override { return TEXT("AudioEnvironmentSystem"); }

    /** Returns the CDO (singleton) instance. Never returns null in a valid game context. */
    static const UAudioEnvironmentSettings* Get();
};
