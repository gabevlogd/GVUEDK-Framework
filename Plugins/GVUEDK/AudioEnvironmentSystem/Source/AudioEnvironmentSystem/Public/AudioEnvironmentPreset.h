#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AudioEnvironmentTypes.h"
#include "AudioEnvironmentPreset.generated.h"

/**
 * Data asset defining the acoustic character of an environment zone.
 *
 * This is intentionally middleware-agnostic. Parameter names (keys) are
 * agreed upon by convention between this asset and whatever backend
 * implementation consumes them. No FMOD, Wwise, or Unreal Audio concepts
 * are hardcoded here.
 *
 * Typical usage:
 *   Create one UAudioEnvironmentPreset per distinct acoustic environment:
 *     - "CaveDeep"        ReverbAmount=0.9, OcclusionFactor=0.8
 *     - "ForestExterior"  ReverbAmount=0.1, ExteriorFactor=1.0, WindIntensity=0.4
 *     - "IndoorSmall"     ReverbAmount=0.4, OcclusionFactor=0.0
 *
 * Extension:
 *   Subclass UAudioEnvironmentPreset and override GetParameters() to generate
 *   parameters procedurally (e.g. weather-driven, time-of-day driven).
 */
UCLASS(BlueprintType, Blueprintable,
    meta = (DisplayName = "Audio Environment Preset"))
class AUDIOENVIRONMENTSYSTEM_API UAudioEnvironmentPreset : public UDataAsset
{
    GENERATED_BODY()

public:
    UAudioEnvironmentPreset();

    // --------------------------------------------------------
    //  Identity
    // --------------------------------------------------------

    /**
     * Human-readable name for this environment, used in debug display and logging.
     * Does not need to match the asset name.
     * Example: "CaveDeep", "ForestExterior", "IndoorLarge"
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment|Identity",
        meta = (ToolTip = "Human-readable label for debug display. E.g. 'CaveDeep', 'ForestExterior'."))
    FName EnvironmentName = NAME_None;

    // --------------------------------------------------------
    //  Layer
    // --------------------------------------------------------

    /**
     * Optional layer tag for grouped blending.
     *
     * Volumes sharing the same LayerTag are blended amongst themselves
     * before competing with volumes in other layers. This allows you to model
     * independent acoustic dimensions (e.g. a "Weather" layer and a "Room" layer)
     * that compose cleanly.
     *
     * Volumes with NAME_None are treated as belonging to a default unnamed layer.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment|Layer",
        meta = (ToolTip = "Volumes sharing this tag blend together before competing with other layers."))
    FName LayerTag = NAME_None;

    // --------------------------------------------------------
    //  Parameters
    // --------------------------------------------------------

    /**
     * Named float parameters for this environment.
     *
     * Key naming is a contract between this asset and your audio backend.
     * The system does not interpret or validate keys in any way.
     *
     * All values are blended linearly when multiple environments overlap.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment|Parameters",
        meta = (ToolTip = "Named float parameters. Keys must match what your audio backend expects."))
    FAudioEnvironmentParameters Parameters;

    // --------------------------------------------------------
    //  API
    // --------------------------------------------------------

    /**
     * Returns the resolved parameters for this preset.
     * Override in subclasses to generate parameters procedurally.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    virtual FAudioEnvironmentParameters GetParameters() const;

    /** Returns the layer tag assigned to this preset. */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    FName GetLayerTag() const { return LayerTag; }

    /** Returns the display name of this preset. */
    UFUNCTION(BlueprintCallable, Category = "Audio|Environment")
    FName GetEnvironmentName() const { return EnvironmentName; }

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
