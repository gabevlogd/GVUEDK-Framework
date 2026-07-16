// Copyright (c) 2024. All Rights Reserved.

#include "AudioEnvironmentBlender.h"
#include "AudioEnvironmentVolume.h"

// ============================================================
//  Blend — dispatch
// ============================================================

FAudioEnvironmentParameters UAudioEnvironmentBlender::Blend(
    const TArray<FAudioEnvironmentVolumeWeight>& InVolumeWeights,
    EAudioEnvironmentPriorityMode InPriorityMode)
{
    switch (InPriorityMode)
    {
    case EAudioEnvironmentPriorityMode::Additive:
        return AdditiveBlend(InVolumeWeights);

    case EAudioEnvironmentPriorityMode::Override:
        return OverrideBlend(InVolumeWeights);

    default:
        return AdditiveBlend(InVolumeWeights);
    }
}

// ============================================================
//  AdditiveBlend — layer-aware weighted sum
// ============================================================

FAudioEnvironmentParameters UAudioEnvironmentBlender::AdditiveBlend(
    const TArray<FAudioEnvironmentVolumeWeight>& InVolumeWeights)
{
    if (InVolumeWeights.IsEmpty())
    {
        return FAudioEnvironmentParameters{};
    }

    // ── Step 1: Group volumes by LayerTag ─────────────────────────────────
    //
    // TMap<LayerTag, (total weight, per-volume entries)>
    struct FLayerAccumulator
    {
        float TotalWeight = 0.f;
        TArray<FWeightedParamEntry> Entries;
    };

    TMap<FName, FLayerAccumulator> Layers;

    for (const FAudioEnvironmentVolumeWeight& VW : InVolumeWeights)
    {
        if (!VW.IsValid())
        {
            continue;
        }

        FLayerAccumulator& Layer = Layers.FindOrAdd(VW.LayerTag);
        Layer.TotalWeight += VW.Weight;

        FWeightedParamEntry& Entry = Layer.Entries.AddDefaulted_GetRef();
        Entry.Params  = VW.Volume->GetParameters();
        Entry.Weight  = VW.Weight;
    }

    if (Layers.IsEmpty())
    {
        return FAudioEnvironmentParameters{};
    }

    // ── Step 2: Blend within each layer (normalize intra-layer weights) ───

    // Holds one blended result per layer, keyed by the layer's total weight.
    TArray<FWeightedParamEntry> LayerResults;
    LayerResults.Reserve(Layers.Num());

    for (auto& LayerPair : Layers)
    {
        FLayerAccumulator& Layer = LayerPair.Value;

        if (Layer.TotalWeight <= KINDA_SMALL_NUMBER)
        {
            continue;
        }

        // Normalize intra-layer weights so they sum to 1.
        const float InvLayerWeight = 1.f / Layer.TotalWeight;
        for (FWeightedParamEntry& Entry : Layer.Entries)
        {
            Entry.Weight *= InvLayerWeight;
        }

        FWeightedParamEntry& LayerResult = LayerResults.AddDefaulted_GetRef();
        LayerResult.Params  = WeightedSum(Layer.Entries);
        LayerResult.Weight  = Layer.TotalWeight;   // Raw weight for inter-layer step.
    }

    // ── Step 3: Blend layers together using their combined weights ────────

    float TotalLayerWeight = 0.f;
    for (const FWeightedParamEntry& LR : LayerResults)
    {
        TotalLayerWeight += LR.Weight;
    }

    if (TotalLayerWeight <= KINDA_SMALL_NUMBER)
    {
        return FAudioEnvironmentParameters{};
    }

    // Normalize inter-layer weights.
    for (FWeightedParamEntry& LR : LayerResults)
    {
        LR.Weight /= TotalLayerWeight;
    }

    return WeightedSum(LayerResults);
}

// ============================================================
//  OverrideBlend
// ============================================================

FAudioEnvironmentParameters UAudioEnvironmentBlender::OverrideBlend(
    const TArray<FAudioEnvironmentVolumeWeight>& InVolumeWeights)
{
    // Find the maximum priority among all valid entries.
    int32 MaxPriority = TNumericLimits<int32>::Lowest();

    for (const FAudioEnvironmentVolumeWeight& VW : InVolumeWeights)
    {
        if (VW.IsValid() && VW.Priority > MaxPriority)
        {
            MaxPriority = VW.Priority;
        }
    }

    if (MaxPriority == TNumericLimits<int32>::Lowest())
    {
        return FAudioEnvironmentParameters{};
    }

    // Collect only the top-priority volumes and blend them additively.
    TArray<FAudioEnvironmentVolumeWeight> TopGroup;
    for (const FAudioEnvironmentVolumeWeight& VW : InVolumeWeights)
    {
        if (VW.IsValid() && VW.Priority == MaxPriority)
        {
            TopGroup.Add(VW);
        }
    }

    return AdditiveBlend(TopGroup);
}

// ============================================================
//  Curve utilities
// ============================================================

float UAudioEnvironmentBlender::Smoothstep(float InT)
{
    // 3t² - 2t³  (Ken Perlin's classic smoothstep)
    return InT * InT * (3.f - 2.f * InT);
}

float UAudioEnvironmentBlender::ApplyBlendMode(float InNormalizedDepth, EAudioEnvironmentBlendMode InMode)
{
    const float T = FMath::Clamp(InNormalizedDepth, 0.f, 1.f);

    switch (InMode)
    {
    case EAudioEnvironmentBlendMode::Linear:
        return T;

    case EAudioEnvironmentBlendMode::Smoothstep:
        return Smoothstep(T);

    case EAudioEnvironmentBlendMode::Curve:
        // Curve asset not accessible in static context; fall through to Smoothstep.
        return Smoothstep(T);

    default:
        return T;
    }
}

// ============================================================
//  WeightedSum — inner utility
// ============================================================

FAudioEnvironmentParameters UAudioEnvironmentBlender::WeightedSum(
    const TArray<FWeightedParamEntry>& InEntries)
{
    FAudioEnvironmentParameters Result;

    for (const FWeightedParamEntry& Entry : InEntries)
    {
        if (Entry.Weight <= KINDA_SMALL_NUMBER)
        {
            continue;
        }

        for (const TPair<FName, float>& Param : Entry.Params.Values)
        {
            // FindOrAdd initializes missing float values to 0.
            Result.Values.FindOrAdd(Param.Key) += Param.Value * Entry.Weight;
        }
    }

    return Result;
}
