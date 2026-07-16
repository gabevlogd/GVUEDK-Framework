// Copyright (c) 2024. All Rights Reserved.

#include "AudioEnvironmentSettings.h"

UAudioEnvironmentSettings::UAudioEnvironmentSettings()
{
    // Sensible defaults are assigned in the header via UPROPERTY initializers.
    // Constructor left intentionally minimal.
}

const UAudioEnvironmentSettings* UAudioEnvironmentSettings::Get()
{
    return GetDefault<UAudioEnvironmentSettings>();
}
