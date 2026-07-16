// Copyright (c) 2024. All Rights Reserved.

#include "AudioEnvironmentPlayerListener.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UAudioEnvironmentPlayerListener::Initialize(
    APlayerController* InController,
    const FGuid&       InListenerID)
{
    checkf(InController, TEXT("UAudioEnvironmentPlayerListener::Initialize called with a null controller."));

    WeakController = InController;
    ListenerID     = InListenerID;
}

FVector UAudioEnvironmentPlayerListener::GetListenerLocation() const
{
    const APlayerController* PC = WeakController.Get();
    if (!PC)
    {
        return FVector::ZeroVector;
    }

    // Prefer pawn actor location (physically grounded listener).
    if (const APawn* Pawn = PC->GetPawn())
    {
        return Pawn->GetActorLocation();
    }

    // Fallback: use the camera/view point location.
    FVector  ViewLocation;
    FRotator ViewRotation;
    PC->GetPlayerViewPoint(ViewLocation, ViewRotation);
    return ViewLocation;
}

bool UAudioEnvironmentPlayerListener::IsListenerActive() const
{
    return WeakController.IsValid();
}

FString UAudioEnvironmentPlayerListener::GetListenerDebugName() const
{
    if (const APlayerController* PC = WeakController.Get())
    {
        return FString::Printf(TEXT("Player_%s"), *PC->GetName());
    }
    return TEXT("Player_Invalid");
}
