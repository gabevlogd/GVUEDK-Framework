// Copyright (c) 2024. All Rights Reserved.

#include "AudioEnvironmentSystemModule.h"

DEFINE_LOG_CATEGORY(LogAudioEnvironment);

IMPLEMENT_MODULE(FAudioEnvironmentSystemModule, AudioEnvironmentSystem)

void FAudioEnvironmentSystemModule::StartupModule()
{
    UE_LOG(LogAudioEnvironment, Log, TEXT("AudioEnvironmentSystem module started."));
}

void FAudioEnvironmentSystemModule::ShutdownModule()
{
    UE_LOG(LogAudioEnvironment, Log, TEXT("AudioEnvironmentSystem module shut down."));
}
