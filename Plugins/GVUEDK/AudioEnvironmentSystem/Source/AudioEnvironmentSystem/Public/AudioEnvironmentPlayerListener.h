#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IAudioEnvironmentListener.h"
#include "AudioEnvironmentPlayerListener.generated.h"

class APlayerController;

/**
 * Default IAudioEnvironmentListener implementation that tracks a PlayerController's pawn.
 *
 * Created and owned by UAudioEnvironmentSubsystem when
 * RegisterPlayerListener(APlayerController*) is called.
 *
 * Location source (in priority order):
 *   1. Controlled pawn's actor location (most common case).
 *   2. Player view point (camera location) — fallback when pawn is null.
 *   3. FVector::ZeroVector — last resort if the controller is invalid.
 *
 * The listener becomes inactive (IsListenerActive = false) if the
 * PlayerController is garbage collected or destroyed, without requiring
 * any explicit unregistration from game code.
 *
 * Extension:
 *   Subclass this to change the location source (e.g. track audio listener
 *   component position instead of pawn root).
 */
UCLASS(BlueprintType, meta = (DisplayName = "Audio Environment Player Listener"))
class AUDIOENVIRONMENTSYSTEM_API UAudioEnvironmentPlayerListener
    : public UObject
    , public IAudioEnvironmentListener
{
    GENERATED_BODY()

public:
    /**
     * Initializes this listener with a PlayerController and a pre-assigned ID.
     * Called by UAudioEnvironmentSubsystem::RegisterPlayerListener().
     *
     * @param InController   The player controller to track. Must be non-null.
     * @param InListenerID   The stable ID assigned to this listener.
     */
    void Initialize(APlayerController* InController, const FGuid& InListenerID);

    // --------------------------------------------------------
    //  IAudioEnvironmentListener
    // --------------------------------------------------------

    virtual FGuid   GetListenerID()       const override { return ListenerID; }
    virtual FVector GetListenerLocation() const override;
    virtual bool    IsListenerActive()    const override;
    virtual FString GetListenerDebugName() const override;

private:
    /** Weak reference to the player controller (safe against GC / destroy). */
    TWeakObjectPtr<APlayerController> WeakController;

    /** Stable ID assigned at Initialize() time. */
    FGuid ListenerID;
};
