#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IAudioEnvironmentShape.h"
#include "AudioEnvironmentTypes.h"
#include "AudioEnvironmentVolume.generated.h"

class UBoxComponent;
class UAudioEnvironmentPreset;
class UCurveFloat;

/**
 * Actor representing a spatial audio environment zone.
 *
 * Design principles:
 * ─────────────────
 * 1. NO trigger authority.
 *    This volume does NOT use OnComponentBeginOverlap / OnComponentEndOverlap
 *    as the source of truth. Weight is evaluated on-demand from a world-space
 *    location. This makes the system immune to respawn, teleport, and save-load
 *    edge cases — the state is always reconstructed from spatial data.
 *
 * 2. Shape abstraction via IAudioEnvironmentShape.
 *    Box is the default. To add sphere, spline, or convex support:
 *      - Override IsLocationInside / GetNormalizedDepth / GetClosestSurfacePoint
 *      - Or create a subclass that replaces BoxComponent with a different primitive.
 *
 * 3. No audio middleware calls.
 *    The volume only holds data and evaluates weights.
 *    All middleware interaction is handled by IAudioEnvironmentBackend implementations.
 *
 * 4. Priority and layer system.
 *    Priority controls Override-mode resolution.
 *    LayerTag controls layer-group blending in Additive mode.
 *
 * Auto-registration:
 *    The volume self-registers with UAudioEnvironmentSubsystem in BeginPlay
 *    and unregisters in EndPlay. No manual registration required.
 */
UCLASS(BlueprintType, Blueprintable,
    meta = (DisplayName = "Audio Environment Volume"))
class AUDIOENVIRONMENTSYSTEM_API AAudioEnvironmentVolume : public AActor, public IAudioEnvironmentShape
{
    GENERATED_BODY()

public:
    AAudioEnvironmentVolume();

    // --------------------------------------------------------
    //  Components
    // --------------------------------------------------------

protected:
    /** Primary bounding/visualization shape. Scaled in the editor as usual. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBoxComponent> BoxComponent;

public:
    /** Returns the box component (for debug drawing, editor tools, etc.). */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    UBoxComponent* GetBoxComponent() const { return BoxComponent; }

    // --------------------------------------------------------
    //  Identity / Setup
    // --------------------------------------------------------

    /**
     * Preset defining the acoustic parameters for this zone.
     * May be null (the volume contributes an empty parameter set,
     * effectively zeroing out any parameter its weight touches).
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Setup",
        meta = (ToolTip = "Acoustic parameter preset for this zone. Null = zero contribution."))
    TObjectPtr<UAudioEnvironmentPreset> Preset;

    /**
     * Evaluation priority. Used during Override-mode resolution:
     * only the highest-priority active volume (or group) contributes.
     * In Additive mode, this value has no effect on blending.
     * Range: any integer; higher = takes precedence.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Setup",
        meta = (ToolTip = "Priority for Override-mode resolution. Higher = takes precedence."))
    int32 Priority = 0;

    /**
     * Opt this volume out of evaluation without removing it from the level.
     * Useful for scripted sequences that temporarily disable a zone.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Setup",
        meta = (ToolTip = "Disable evaluation of this volume without removing it."))
    bool bEnabled = true;

    // --------------------------------------------------------
    //  Blending
    // --------------------------------------------------------

    /**
     * Blend falloff distance in world units (cm by default).
     *
     * Defines a zone inside the volume boundary where weight transitions
     * from 0 (at the boundary) to 1 (fully inside).
     *
     * - At the box surface:              weight ≈ 0
     * - BlendDistance inside the surface: weight = 1
     * - Beyond BlendDistance inward:      weight = 1 (clamped)
     *
     * Set to 0 for an instant snap (no crossfade at boundary).
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Blending",
        meta = (ClampMin = "0.0", UIMin = "0.0",
            ToolTip = "Distance inside the volume boundary over which weight blends from 0 to 1."))
    float BlendDistance = 300.f;

    /**
     * Blend curve type for the falloff within BlendDistance.
     * Overrides the project default for this specific volume.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Blending",
        meta = (ToolTip = "Falloff curve within the blend zone. Smoothstep is recommended."))
    EAudioEnvironmentBlendMode BlendMode = EAudioEnvironmentBlendMode::Smoothstep;

    /**
     * Custom curve asset. Only evaluated when BlendMode = Curve.
     * X axis = normalized depth [0..1] (0=boundary, 1=deep inside).
     * Y axis = output weight [0..1].
     * Falls back to Smoothstep if this is null.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Blending",
        meta = (EditCondition = "BlendMode == EAudioEnvironmentBlendMode::Curve",
            ToolTip = "Custom weight curve. X=NormalizedDepth, Y=Weight. Both axes should be [0..1]."))
    TObjectPtr<UCurveFloat> BlendCurve;

    // --------------------------------------------------------
    //  Layering
    // --------------------------------------------------------

    /**
     * Layer tag for grouped blending.
     *
     * Volumes sharing the same LayerTag are blended amongst themselves
     * before the merged result competes with other layers.
     * This is automatically synced from Preset->LayerTag when the preset
     * is assigned, but can be overridden here.
     *
     * NAME_None = default (unnamed) layer.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Layer",
        meta = (ToolTip = "Volumes with the same LayerTag blend together before inter-layer competition."))
    FName LayerTag = NAME_None;

    // --------------------------------------------------------
    //  Evaluation API
    // --------------------------------------------------------

    /**
     * Evaluates and returns the blend weight for a given world-space location.
     *
     * Weight = 0 if:
     *   - The volume is disabled (bEnabled = false)
     *   - The location is outside the shape boundary
     *
     * Weight transitions from 0→1 within BlendDistance of the boundary,
     * using the configured BlendMode curve.
     *
     * Weight = 1 if the location is deeper than BlendDistance from any face.
     *
     * @param InWorldLocation  Location to evaluate (typically: listener position).
     * @return                 Normalized weight in [0..1].
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    float EvaluateWeightAtLocation(const FVector& InWorldLocation) const;

    /**
     * Returns the resolved parameter set for this volume.
     * Reads from Preset->GetParameters(), or returns an empty set if no preset.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    FAudioEnvironmentParameters GetParameters() const;

    /** Returns this volume's configured priority. */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    int32 GetPriority() const { return Priority; }

    /** Returns whether this volume is currently enabled for evaluation. */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    bool IsEnabled() const { return bEnabled; }

    // --------------------------------------------------------
    //  IAudioEnvironmentShape (Box default implementation)
    // --------------------------------------------------------

    virtual bool         IsLocationInside(const FVector& InWorldLocation) const override;
    virtual float        GetNormalizedDepth(const FVector& InWorldLocation, float InBlendDistance) const override;
    virtual FVector      GetClosestSurfacePoint(const FVector& InWorldLocation) const override;
    virtual FBoxSphereBounds GetShapeBounds() const override;

    // --------------------------------------------------------
    //  AActor overrides
    // --------------------------------------------------------

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void OnConstruction(const FTransform& Transform) override;
#endif

protected:
    /**
     * Applies BlendMode to a raw normalized depth value to produce the final weight.
     * Dispatches to linear ramp, smoothstep, or the BlendCurve asset.
     */
    float ApplyBlendCurve(float InNormalizedDepth) const;

    /**
     * Copies LayerTag from the assigned Preset.
     * Only overwrites if LayerTag is currently NAME_None, to allow manual overrides.
     */
    void SyncLayerTagFromPreset();
};
