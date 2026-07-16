// Copyright (c) 2024. All Rights Reserved.

#include "AudioEnvironmentEvaluator.h"

#include "AudioEnvironmentVolume.h"
#include "AudioEnvironmentBlender.h"
#include "AudioEnvironmentSystemModule.h"

// ============================================================
//  Evaluate
// ============================================================

FAudioEnvironmentListenerState UAudioEnvironmentEvaluator::Evaluate(
    const FGuid&                                           InListenerID,
    const FVector&                                         InListenerLocation,
    const TArray<TWeakObjectPtr<AAudioEnvironmentVolume>>& InVolumes,
    int32                                                  InMaxVolumes,
    EAudioEnvironmentPriorityMode                          InPriorityMode,
    float                                                  InCurrentTime) const
{
    FAudioEnvironmentListenerState State;
    State.ListenerID      = InListenerID;
    State.Location        = InListenerLocation;
    State.EvaluationTime  = InCurrentTime;

    // Gather and sort active volume weights.
    State.ActiveVolumes = GatherVolumeWeights(InListenerLocation, InVolumes, InMaxVolumes);

    // Blend all active volumes into a single parameter set.
    State.BlendedParameters = UAudioEnvironmentBlender::Blend(State.ActiveVolumes, InPriorityMode);

    return State;
}

// ============================================================
//  GatherVolumeWeights
// ============================================================

TArray<FAudioEnvironmentVolumeWeight> UAudioEnvironmentEvaluator::GatherVolumeWeights(
    const FVector&                                         InLocation,
    const TArray<TWeakObjectPtr<AAudioEnvironmentVolume>>& InVolumes,
    int32                                                  InMaxVolumes) const
{
    // ── Phase 1: Collect candidates with bounding sphere distance ─────────
    //
    // We sort by how close the bounding sphere is to InLocation.
    // This ensures the InMaxVolumes budget is spent on the most relevant volumes.

    struct FCandidate
    {
        AAudioEnvironmentVolume* Volume      = nullptr;
        float                   ProximitySq = 0.f;  // Rough sorting metric only.
    };

    TArray<FCandidate> Candidates;
    Candidates.Reserve(InVolumes.Num());

    for (const TWeakObjectPtr<AAudioEnvironmentVolume>& WeakVolume : InVolumes)
    {
        AAudioEnvironmentVolume* Volume = WeakVolume.Get();

        if (!Volume || !Volume->IsEnabled())
        {
            continue;
        }

        const FBoxSphereBounds Bounds    = Volume->GetShapeBounds();
        const float            DistSq    = FVector::DistSquared(InLocation, Bounds.Origin);
        const float            RadiusSq  = Bounds.SphereRadius * Bounds.SphereRadius;

        // Conservative proximity metric: only volumes whose bounding sphere
        // could possibly contain InLocation are candidates.
        if (DistSq > RadiusSq)
        {
            // Location is outside the bounding sphere — weight is guaranteed 0.
            continue;
        }

        Candidates.Add({ Volume, DistSq });
    }

    // Sort by proximity (nearest first) for budget allocation.
    Candidates.Sort([](const FCandidate& A, const FCandidate& B)
    {
        return A.ProximitySq < B.ProximitySq;
    });

    // Apply evaluation budget.
    if (Candidates.Num() > InMaxVolumes)
    {
        Candidates.SetNum(InMaxVolumes);
    }

    // ── Phase 2: Evaluate actual blend weight ─────────────────────────────

    TArray<FAudioEnvironmentVolumeWeight> Result;
    Result.Reserve(Candidates.Num());

    for (const FCandidate& Candidate : Candidates)
    {
        const float Weight = Candidate.Volume->EvaluateWeightAtLocation(InLocation);

        if (Weight > KINDA_SMALL_NUMBER)
        {
            FAudioEnvironmentVolumeWeight& VW = Result.AddDefaulted_GetRef();
            VW.Volume   = Candidate.Volume;
            VW.Weight   = Weight;
            VW.Priority = Candidate.Volume->GetPriority();
            VW.LayerTag = Candidate.Volume->LayerTag;
        }
    }

    // Sort active volumes by descending weight for consistent debug output.
    Result.Sort([](const FAudioEnvironmentVolumeWeight& A, const FAudioEnvironmentVolumeWeight& B)
    {
        return A.Weight > B.Weight;
    });

    return Result;
}

// ============================================================
//  HasStateChangedSignificantly
// ============================================================

bool UAudioEnvironmentEvaluator::HasStateChangedSignificantly(
    const FAudioEnvironmentListenerState& InPrev,
    const FAudioEnvironmentListenerState& InNext,
    float InThreshold) const
{
    // Fast path: active volume count changed.
    if (InPrev.ActiveVolumes.Num() != InNext.ActiveVolumes.Num())
    {
        return true;
    }

    // Check for active volume set membership change (same count but different volumes).
    for (int32 i = 0; i < InNext.ActiveVolumes.Num(); ++i)
    {
        if (InNext.ActiveVolumes[i].Volume != InPrev.ActiveVolumes[i].Volume)
        {
            return true;
        }
    }

    // Check blended parameter deltas.
    const FAudioEnvironmentParameters& PrevParams = InPrev.BlendedParameters;
    const FAudioEnvironmentParameters& NextParams = InNext.BlendedParameters;

    // Keys present in next but not in prev (or with meaningful delta).
    for (const TPair<FName, float>& NextPair : NextParams.Values)
    {
        const float PrevValue = PrevParams.GetParameter(NextPair.Key, 0.f);
        if (FMath::Abs(NextPair.Value - PrevValue) > InThreshold)
        {
            return true;
        }
    }

    // Keys present in prev but gone from next.
    for (const TPair<FName, float>& PrevPair : PrevParams.Values)
    {
        if (!NextParams.HasParameter(PrevPair.Key))
        {
            if (FMath::Abs(PrevPair.Value) > InThreshold)
            {
                return true;
            }
        }
    }

    return false;
}
