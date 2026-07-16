#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAudioEnvironmentListener.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable,
    meta = (ToolTip = "Implement this interface to register any object as an audio environment listener."))
class UAudioEnvironmentListener : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for audio environment listeners.
 *
 * Any UObject (pawn, actor component, camera, etc.) can act as a listener
 * by implementing this interface and registering with UAudioEnvironmentSubsystem.
 *
 * This abstraction enables:
 *   - Multiple simultaneous listeners (split-screen, spectator)
 *   - Non-pawn listeners (e.g. audio camera)
 *   - Blueprint-defined listeners
 *   - Future network-replicated listeners
 */
class AUDIOENVIRONMENTSYSTEM_API IAudioEnvironmentListener
{
    GENERATED_BODY()

public:
    /**
     * Returns a stable GUID that uniquely identifies this listener.
     * Must remain constant for the lifetime of the listener registration.
     */
    virtual FGuid GetListenerID() const = 0;

    /**
     * Returns the current world-space location from which environments
     * should be evaluated. Called every evaluation tick.
     */
    virtual FVector GetListenerLocation() const = 0;

    /**
     * Returns false to temporarily pause evaluation for this listener
     * without unregistering it (e.g. during loading, cutscenes).
     * Default: always active.
     */
    virtual bool IsListenerActive() const { return true; }

    /**
     * Optional human-readable name for debug visualization.
     * Default: "Listener".
     */
    virtual FString GetListenerDebugName() const { return TEXT("Listener"); }
};
