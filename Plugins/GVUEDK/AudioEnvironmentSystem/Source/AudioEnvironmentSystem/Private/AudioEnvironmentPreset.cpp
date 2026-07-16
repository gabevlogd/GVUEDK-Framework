// Copyright (c) 2024. All Rights Reserved.

#include "AudioEnvironmentPreset.h"
#include "AudioEnvironmentSystemModule.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UAudioEnvironmentPreset::UAudioEnvironmentPreset()
{
    EnvironmentName = NAME_None;
    LayerTag = NAME_None;
}

FAudioEnvironmentParameters UAudioEnvironmentPreset::GetParameters() const
{
    return Parameters;
}

#if WITH_EDITOR

void UAudioEnvironmentPreset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName ChangedProp = PropertyChangedEvent.GetPropertyName();

    UE_LOG(LogAudioEnvironment, Verbose,
        TEXT("AudioEnvironmentPreset '%s': property '%s' changed."),
        *GetName(), *ChangedProp.ToString());
}

EDataValidationResult UAudioEnvironmentPreset::IsDataValid(FDataValidationContext& Context) const
{
    EDataValidationResult Result = Super::IsDataValid(Context);

    if (EnvironmentName.IsNone())
    {
        Context.AddWarning(FText::FromString(
            FString::Printf(TEXT("AudioEnvironmentPreset '%s' has no EnvironmentName set. "
                "Set one for cleaner debug output."), *GetName())));
    }

    if (Parameters.Values.IsEmpty())
    {
        Context.AddWarning(FText::FromString(
            FString::Printf(TEXT("AudioEnvironmentPreset '%s' has no parameters defined. "
                "Add at least one named float parameter."), *GetName())));
    }

    return Result;
}

#endif // WITH_EDITOR
