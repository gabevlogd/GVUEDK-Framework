// Copyright (c) 2024. All Rights Reserved.

#include "AudioEnvironmentTypes.h"

// ============================================================
//  FAudioEnvironmentParameters
// ============================================================

void FAudioEnvironmentParameters::SetParameter(FName InKey, float InValue)
{
    Values.FindOrAdd(InKey) = InValue;
}

float FAudioEnvironmentParameters::GetParameter(FName InKey, float InDefault) const
{
    const float* Found = Values.Find(InKey);
    return Found ? *Found : InDefault;
}

bool FAudioEnvironmentParameters::HasParameter(FName InKey) const
{
    return Values.Contains(InKey);
}

void FAudioEnvironmentParameters::MergeFrom(const FAudioEnvironmentParameters& InOther)
{
    for (const TPair<FName, float>& Pair : InOther.Values)
    {
        Values.FindOrAdd(Pair.Key) = Pair.Value;
    }
}

void FAudioEnvironmentParameters::MergeDefaults(const FAudioEnvironmentParameters& InDefaults)
{
    for (const TPair<FName, float>& Pair : InDefaults.Values)
    {
        if (!Values.Contains(Pair.Key))
        {
            Values.Add(Pair.Key, Pair.Value);
        }
    }
}

FAudioEnvironmentParameters FAudioEnvironmentParameters::Lerp(
    const FAudioEnvironmentParameters& InA,
    const FAudioEnvironmentParameters& InB,
    float InAlpha)
{
    FAudioEnvironmentParameters Result;

    // Keys present in A
    for (const TPair<FName, float>& Pair : InA.Values)
    {
        const float BValue = InB.GetParameter(Pair.Key, Pair.Value);
        Result.Values.Add(Pair.Key, FMath::Lerp(Pair.Value, BValue, InAlpha));
    }

    // Keys present only in B (treat A value as 0)
    for (const TPair<FName, float>& Pair : InB.Values)
    {
        if (!InA.Values.Contains(Pair.Key))
        {
            Result.Values.Add(Pair.Key, FMath::Lerp(0.f, Pair.Value, InAlpha));
        }
    }

    return Result;
}

bool FAudioEnvironmentParameters::operator==(const FAudioEnvironmentParameters& Other) const
{
    if (Values.Num() != Other.Values.Num())
    {
        return false;
    }

    for (const TPair<FName, float>& Pair : Values)
    {
        const float* OtherVal = Other.Values.Find(Pair.Key);
        if (!OtherVal || !FMath::IsNearlyEqual(Pair.Value, *OtherVal, KINDA_SMALL_NUMBER))
        {
            return false;
        }
    }

    return true;
}
