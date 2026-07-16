// Copyright (c) 2024. All Rights Reserved.

#include "AudioEnvironmentSubsystem.h"

#include "AudioEnvironmentEvaluator.h"
#include "AudioEnvironmentSettings.h"
#include "AudioEnvironmentVolume.h"
#include "AudioEnvironmentPlayerListener.h"
#include "AudioEnvironmentPreset.h"
#include "IAudioEnvironmentListener.h"
#include "IAudioEnvironmentBackend.h"
#include "AudioEnvironmentSystemModule.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Stats/Stats.h"
#include "UObject/ScriptInterface.h"

// ============================================================
//  Console variables
// ============================================================

static TAutoConsoleVariable<int32> CVarAudioEnvDebugDraw(
    TEXT("AudioEnv.DebugDraw"),
    0,
    TEXT("Enable Audio Environment System debug visualization.\n")
    TEXT("  0 = Off\n")
    TEXT("  1 = On (volumes + listener state)"),
    ECVF_Default);

static TAutoConsoleVariable<int32> CVarAudioEnvVerbose(
    TEXT("AudioEnv.Verbose"),
    0,
    TEXT("Log detailed evaluation data each tick (0=Off, 1=On). High volume."),
    ECVF_Default);

// ============================================================
//  FListenerEntry helpers
// ============================================================

IAudioEnvironmentListener* UAudioEnvironmentSubsystem::FListenerEntry::GetInterface() const
{
    UObject* Obj = WeakObject.Get();
    return Obj ? Cast<IAudioEnvironmentListener>(Obj) : nullptr;
}

// ============================================================
//  UWorldSubsystem / UTickableWorldSubsystem
// ============================================================

bool UAudioEnvironmentSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer))
    {
        return false;
    }

    const UWorld* World = Cast<UWorld>(Outer);
    return World && (World->IsGameWorld() || World->IsPlayInEditor());
}

void UAudioEnvironmentSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    Evaluator = NewObject<UAudioEnvironmentEvaluator>(this);
    TimeSinceLastEvaluation = 0.f;

    // Read initial debug setting from project settings.
    if (const UAudioEnvironmentSettings* Settings = UAudioEnvironmentSettings::Get())
    {
        bDebugVisualizationEnabled = Settings->bEnableDebugVisualization;
    }

    UE_LOG(LogAudioEnvironment, Log,
        TEXT("UAudioEnvironmentSubsystem initialized for world '%s'."),
        *GetWorld()->GetName());
}

void UAudioEnvironmentSubsystem::Deinitialize()
{
    // Notify and release all backends cleanly.
    for (const TWeakObjectPtr<UObject>& WeakBackend : RegisteredBackends)
    {
        if (UObject* Obj = WeakBackend.Get())
        {
            if (IAudioEnvironmentBackend* Backend = Cast<IAudioEnvironmentBackend>(Obj))
            {
                Backend->Shutdown();
            }
        }
    }

    RegisteredVolumes.Empty();
    RegisteredListeners.Empty();
    RegisteredBackends.Empty();
    OwnedPlayerListeners.Empty();

    UE_LOG(LogAudioEnvironment, Log,
        TEXT("UAudioEnvironmentSubsystem shut down."));

    Super::Deinitialize();
}

TStatId UAudioEnvironmentSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UAudioEnvironmentSubsystem, STATGROUP_Tickables);
}

// ============================================================
//  Tick
// ============================================================

void UAudioEnvironmentSubsystem::Tick(float DeltaTime)
{
    TimeSinceLastEvaluation += DeltaTime;

    const UAudioEnvironmentSettings* Settings = UAudioEnvironmentSettings::Get();
    const float Interval = Settings ? Settings->EvaluationInterval : 0.1f;

    if (TimeSinceLastEvaluation >= Interval)
    {
        TimeSinceLastEvaluation = 0.f;

        CleanupStaleVolumes();
        CleanupStaleListeners();
        CleanupStaleBackends();

        EvaluateAllListeners();
    }

    // Debug draw runs every frame (not just on evaluation ticks) for smooth rendering.
    const bool bShouldDraw = bDebugVisualizationEnabled
        || (CVarAudioEnvDebugDraw.GetValueOnGameThread() != 0);

    if (bShouldDraw)
    {
        DrawDebug();
    }
}

// ============================================================
//  Volume registration
// ============================================================

void UAudioEnvironmentSubsystem::RegisterVolume(AAudioEnvironmentVolume* InVolume)
{
    if (!InVolume)
    {
        return;
    }

    // Prevent duplicate registrations.
    for (const TWeakObjectPtr<AAudioEnvironmentVolume>& Existing : RegisteredVolumes)
    {
        if (Existing.Get() == InVolume)
        {
            UE_LOG(LogAudioEnvironment, Verbose,
                TEXT("RegisterVolume: '%s' is already registered. Skipping."), *InVolume->GetName());
            return;
        }
    }

    RegisteredVolumes.Add(InVolume);

    UE_LOG(LogAudioEnvironment, Verbose,
        TEXT("Volume registered: '%s'. Total: %d."), *InVolume->GetName(), RegisteredVolumes.Num());
}

void UAudioEnvironmentSubsystem::UnregisterVolume(AAudioEnvironmentVolume* InVolume)
{
    const int32 Removed = RegisteredVolumes.RemoveAll(
        [InVolume](const TWeakObjectPtr<AAudioEnvironmentVolume>& WeakVol)
        {
            return WeakVol.Get() == InVolume || !WeakVol.IsValid();
        });

    if (Removed > 0)
    {
        UE_LOG(LogAudioEnvironment, Verbose,
            TEXT("Volume unregistered: '%s'. Remaining: %d."),
            InVolume ? *InVolume->GetName() : TEXT("null"),
            RegisteredVolumes.Num());
    }
}

// ============================================================
//  Listener registration
// ============================================================

FGuid UAudioEnvironmentSubsystem::RegisterListener(
    TScriptInterface<IAudioEnvironmentListener> InListener)
{
    UObject* Obj = InListener.GetObject();
    if (!Obj)
    {
        UE_LOG(LogAudioEnvironment, Warning, TEXT("RegisterListener: null object passed. Ignored."));
        return FGuid{};
    }

    if (!Cast<IAudioEnvironmentListener>(Obj))
    {
        UE_LOG(LogAudioEnvironment, Warning,
            TEXT("RegisterListener: object '%s' does not implement IAudioEnvironmentListener."),
            *Obj->GetName());
        return FGuid{};
    }

    FListenerEntry& Entry = RegisteredListeners.AddDefaulted_GetRef();
    Entry.ListenerID = FGuid::NewGuid();
    Entry.WeakObject = Obj;

    UE_LOG(LogAudioEnvironment, Log,
        TEXT("Listener registered: '%s' [ID=%s]."),
        *Obj->GetName(),
        *Entry.ListenerID.ToString(EGuidFormats::Short));

    return Entry.ListenerID;
}

FGuid UAudioEnvironmentSubsystem::RegisterPlayerListener(APlayerController* InController)
{
    if (!InController)
    {
        UE_LOG(LogAudioEnvironment, Warning,
            TEXT("RegisterPlayerListener: null PlayerController passed. Ignored."));
        return FGuid{};
    }

    // Create and own the wrapper listener.
    UAudioEnvironmentPlayerListener* PlayerListener =
        NewObject<UAudioEnvironmentPlayerListener>(this);

    const FGuid NewID = FGuid::NewGuid();
    PlayerListener->Initialize(InController, NewID);

    // Keep alive via UPROPERTY array.
    OwnedPlayerListeners.Add(PlayerListener);

    // Register using the generic path.
    FListenerEntry& Entry = RegisteredListeners.AddDefaulted_GetRef();
    Entry.ListenerID = NewID;
    Entry.WeakObject = PlayerListener;

    UE_LOG(LogAudioEnvironment, Log,
        TEXT("Player listener registered for '%s' [ID=%s]."),
        *InController->GetName(),
        *NewID.ToString(EGuidFormats::Short));

    return NewID;
}

void UAudioEnvironmentSubsystem::UnregisterListener(const FGuid& InListenerID)
{
    const int32 Removed = RegisteredListeners.RemoveAll(
        [&InListenerID](const FListenerEntry& Entry)
        {
            return Entry.ListenerID == InListenerID;
        });

    // Also remove any associated player listener wrappers.
    OwnedPlayerListeners.RemoveAll([](const TObjectPtr<UAudioEnvironmentPlayerListener>& PL)
    {
        return PL == nullptr;
    });

    UE_LOG(LogAudioEnvironment, Verbose,
        TEXT("Listener [ID=%s] unregistered (%d removed)."),
        *InListenerID.ToString(EGuidFormats::Short), Removed);
}

// ============================================================
//  Backend registration
// ============================================================

bool UAudioEnvironmentSubsystem::RegisterBackend(UObject* InBackendObject)
{
    if (!InBackendObject)
    {
        return false;
    }

    if (!Cast<IAudioEnvironmentBackend>(InBackendObject))
    {
        UE_LOG(LogAudioEnvironment, Warning,
            TEXT("RegisterBackend: '%s' does not implement IAudioEnvironmentBackend. Ignored."),
            *InBackendObject->GetName());
        return false;
    }

    RegisteredBackends.AddUnique(InBackendObject);

    if (IAudioEnvironmentBackend* Backend = Cast<IAudioEnvironmentBackend>(InBackendObject))
    {
        Backend->Initialize();
    }

    UE_LOG(LogAudioEnvironment, Log,
        TEXT("Backend registered: '%s'."), *InBackendObject->GetName());
    
    return true;
}

void UAudioEnvironmentSubsystem::UnregisterBackend(UObject* InBackendObject)
{
    if (!InBackendObject)
    {
        return;
    }

    if (IAudioEnvironmentBackend* Backend = Cast<IAudioEnvironmentBackend>(InBackendObject))
    {
        Backend->Shutdown();
    }

    RegisteredBackends.RemoveAll([InBackendObject](const TWeakObjectPtr<UObject>& WeakB)
    {
        return WeakB.Get() == InBackendObject || !WeakB.IsValid();
    });
}

// ============================================================
//  Query API
// ============================================================

bool UAudioEnvironmentSubsystem::GetListenerState(
    const FGuid& InListenerID,
    FAudioEnvironmentListenerState& OutState) const
{
    for (const FListenerEntry& Entry : RegisteredListeners)
    {
        if (Entry.ListenerID == InListenerID)
        {
            OutState = Entry.LastState;
            return true;
        }
    }
    return false;
}

bool UAudioEnvironmentSubsystem::GetPrimaryListenerState(
    FAudioEnvironmentListenerState& OutState) const
{
    if (RegisteredListeners.IsEmpty())
    {
        return false;
    }
    OutState = RegisteredListeners[0].LastState;
    return true;
}

TArray<FAudioEnvironmentVolumeWeight> UAudioEnvironmentSubsystem::GetActiveVolumesForListener(
    const FGuid& InListenerID) const
{
    for (const FListenerEntry& Entry : RegisteredListeners)
    {
        if (Entry.ListenerID == InListenerID)
        {
            return Entry.LastState.ActiveVolumes;
        }
    }
    return {};
}

void UAudioEnvironmentSubsystem::ForceEvaluate()
{
    CleanupStaleVolumes();
    CleanupStaleListeners();
    CleanupStaleBackends();

    EvaluateAllListeners();

    TimeSinceLastEvaluation = 0.f;

    UE_LOG(LogAudioEnvironment, Verbose, TEXT("ForceEvaluate() called. All listeners re-evaluated."));
}

// ============================================================
//  Debug
// ============================================================

void UAudioEnvironmentSubsystem::SetDebugVisualizationEnabled(bool bInEnabled)
{
    bDebugVisualizationEnabled = bInEnabled;
}

// ============================================================
//  Internal — Evaluation
// ============================================================

void UAudioEnvironmentSubsystem::EvaluateAllListeners()
{
    for (FListenerEntry& Entry : RegisteredListeners)
    {
        EvaluateListener(Entry);
    }
}

void UAudioEnvironmentSubsystem::EvaluateListener(FListenerEntry& InOutEntry)
{
    IAudioEnvironmentListener* ListenerInterface = InOutEntry.GetInterface();

    if (!ListenerInterface)
    {
        return;   // Object was destroyed — will be cleaned up next cleanup pass.
    }

    if (!ListenerInterface->IsListenerActive())
    {
        return;   // Listener paused (loading, cutscene, etc.).
    }

    const UAudioEnvironmentSettings* Settings        = UAudioEnvironmentSettings::Get();
    const int32 MaxVolumes                           = Settings ? Settings->MaxVolumesPerListener : 16;
    const EAudioEnvironmentPriorityMode PriorityMode = Settings
        ? Settings->DefaultPriorityMode
        : EAudioEnvironmentPriorityMode::Additive;
    const float ChangeThreshold                      = Settings
        ? Settings->ChangeNotificationThreshold
        : 0.001f;

    const FVector ListenerLocation = ListenerInterface->GetListenerLocation();
    const float   CurrentTime      = GetWorld()->GetTimeSeconds();

    FAudioEnvironmentListenerState NewState = Evaluator->Evaluate(
        InOutEntry.ListenerID,
        ListenerLocation,
        RegisteredVolumes,
        MaxVolumes,
        PriorityMode,
        CurrentTime);

    // Always broadcast tick (used for continuous backend smoothing).
    NotifyStateTick(NewState);
    OnEnvironmentTick.Broadcast(NewState.ListenerID, NewState);

    // Only broadcast changed if something meaningful happened.
    const bool bChanged = Evaluator->HasStateChangedSignificantly(
        InOutEntry.LastState, NewState, ChangeThreshold);

    if (bChanged)
    {
        NotifyStateChanged(InOutEntry.LastState, NewState);
        OnEnvironmentStateChanged.Broadcast(NewState.ListenerID, NewState);

        if (CVarAudioEnvVerbose.GetValueOnGameThread() != 0)
        {
            UE_LOG(LogAudioEnvironment, Log,
                TEXT("State changed for listener [%s] at %s | ActiveVolumes=%d"),
                *InOutEntry.ListenerID.ToString(EGuidFormats::Short),
                *ListenerLocation.ToString(),
                NewState.ActiveVolumes.Num());

            for (const auto& Param : NewState.BlendedParameters.Values)
            {
                UE_LOG(LogAudioEnvironment, Log,
                    TEXT("  %s = %.4f"), *Param.Key.ToString(), Param.Value);
            }
        }
    }

    InOutEntry.LastState = MoveTemp(NewState);
}

// ============================================================
//  Internal — Notification
// ============================================================

void UAudioEnvironmentSubsystem::NotifyStateChanged(
    const FAudioEnvironmentListenerState& InPrev,
    const FAudioEnvironmentListenerState& InNew)
{
    for (const TWeakObjectPtr<UObject>& WeakBackend : RegisteredBackends)
    {
        if (UObject* Obj = WeakBackend.Get())
        {
            if (IAudioEnvironmentBackend* Backend = Cast<IAudioEnvironmentBackend>(Obj))
            {
                Backend->OnListenerEnvironmentChanged(InNew.ListenerID, InPrev, InNew);
            }
        }
    }
}

void UAudioEnvironmentSubsystem::NotifyStateTick(const FAudioEnvironmentListenerState& InState)
{
    for (const TWeakObjectPtr<UObject>& WeakBackend : RegisteredBackends)
    {
        if (UObject* Obj = WeakBackend.Get())
        {
            if (IAudioEnvironmentBackend* Backend = Cast<IAudioEnvironmentBackend>(Obj))
            {
                Backend->OnListenerEnvironmentTick(InState.ListenerID, InState);
            }
        }
    }
}

// ============================================================
//  Internal — Cleanup
// ============================================================

void UAudioEnvironmentSubsystem::CleanupStaleVolumes()
{
    const int32 Before = RegisteredVolumes.Num();
    RegisteredVolumes.RemoveAll([](const TWeakObjectPtr<AAudioEnvironmentVolume>& WeakVol)
    {
        return !WeakVol.IsValid();
    });
    const int32 Removed = Before - RegisteredVolumes.Num();

    if (Removed > 0)
    {
        UE_LOG(LogAudioEnvironment, Verbose,
            TEXT("Cleaned up %d stale volume reference(s)."), Removed);
    }
}

void UAudioEnvironmentSubsystem::CleanupStaleListeners()
{
    const int32 Before = RegisteredListeners.Num();
    RegisteredListeners.RemoveAll([](const FListenerEntry& Entry)
    {
        return !Entry.IsAlive();
    });
    const int32 Removed = Before - RegisteredListeners.Num();

    if (Removed > 0)
    {
        UE_LOG(LogAudioEnvironment, Verbose,
            TEXT("Cleaned up %d stale listener reference(s)."), Removed);
    }
}

void UAudioEnvironmentSubsystem::CleanupStaleBackends()
{
    RegisteredBackends.RemoveAll([](const TWeakObjectPtr<UObject>& WeakB)
    {
        return !WeakB.IsValid();
    });
}

// ============================================================
//  Debug visualization
// ============================================================

void UAudioEnvironmentSubsystem::DrawDebug() const
{
    const UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const UAudioEnvironmentSettings* Settings   = UAudioEnvironmentSettings::Get();
    const float DrawDuration                    = Settings ? Settings->DebugDrawDuration        : 0.f;
    const FColor ActiveColor                    = Settings
        ? Settings->DebugActiveVolumeColor.ToFColor(true)
        : FColor(0, 220, 50, 180);
    const FColor InactiveColor                  = Settings
        ? Settings->DebugInactiveVolumeColor.ToFColor(true)
        : FColor(255, 130, 0, 80);
    const FColor ListenerColor                  = Settings
        ? Settings->DebugListenerColor.ToFColor(true)
        : FColor::Yellow;

    // Build a quick lookup of which volumes are currently active for any listener.
    TSet<const AAudioEnvironmentVolume*> ActiveVolumes;
    TMap<const AAudioEnvironmentVolume*, float> VolumeWeightMap;

    for (const FListenerEntry& Entry : RegisteredListeners)
    {
        for (const FAudioEnvironmentVolumeWeight& VW : Entry.LastState.ActiveVolumes)
        {
            if (VW.Volume)
            {
                ActiveVolumes.Add(VW.Volume.Get());
                float& MaxW = VolumeWeightMap.FindOrAdd(VW.Volume.Get());
                MaxW = FMath::Max(MaxW, VW.Weight);
            }
        }
    }

    // ── Draw volumes ──────────────────────────────────────────────────────

    for (const TWeakObjectPtr<AAudioEnvironmentVolume>& WeakVol : RegisteredVolumes)
    {
        const AAudioEnvironmentVolume* Vol = WeakVol.Get();
        if (!Vol)
        {
            continue;
        }

        const UBoxComponent* Box = Vol->GetBoxComponent();
        if (!Box)
        {
            continue;
        }

        const bool  bIsActive  = ActiveVolumes.Contains(Vol);
        const float Weight     = bIsActive ? VolumeWeightMap.FindRef(Vol) : 0.f;
        const FColor DrawColor = bIsActive ? ActiveColor : InactiveColor;

        DrawDebugBox(
            World,
            Box->GetComponentLocation(),
            Box->GetScaledBoxExtent(),
            Box->GetComponentRotation().Quaternion(),
            DrawColor,
            /*bPersistentLines=*/false,
            DrawDuration,
            /*DepthPriority=*/0,
            /*Thickness=*/bIsActive ? 3.f : 1.f);

        if (bIsActive)
        {
            // Label: EnvironmentName, Priority, Weight
            const FString PresetName = (Vol->Preset && !Vol->Preset->EnvironmentName.IsNone())
                ? Vol->Preset->EnvironmentName.ToString()
                : TEXT("No Preset");

            const FString Label = FString::Printf(
                TEXT("%s\nP:%d  W:%.2f\nLayer:%s"),
                *PresetName,
                Vol->GetPriority(),
                Weight,
                *Vol->LayerTag.ToString());

            DrawDebugString(
                World,
                Box->GetComponentLocation() + FVector(0.f, 0.f, Box->GetScaledBoxExtent().Z + 20.f),
                Label,
                /*TestBaseActor=*/nullptr,
                DrawColor,
                DrawDuration,
                /*bDrawShadow=*/true);
        }
    }

    // ── Draw listener states ──────────────────────────────────────────────

    for (const FListenerEntry& Entry : RegisteredListeners)
    {
        if (!Entry.LastState.IsValid())
        {
            continue;
        }

        const FVector& Loc = Entry.LastState.Location;

        DrawDebugSphere(World, Loc, 35.f, 12, ListenerColor, false, DrawDuration, 0, 2.f);
        DrawDebugLine(World, Loc, Loc + FVector(0.f, 0.f, 80.f), ListenerColor, false, DrawDuration, 0, 1.5f);

        // Build parameter display string.
        FString ParamStr = FString::Printf(
            TEXT("[%s]\n"),
            *Entry.ListenerID.ToString(EGuidFormats::Short));

        const FAudioEnvironmentParameters& Params = Entry.LastState.BlendedParameters;
        if (Params.Values.IsEmpty())
        {
            ParamStr += TEXT("(no active environment)");
        }
        else
        {
            for (const TPair<FName, float>& Param : Params.Values)
            {
                ParamStr += FString::Printf(TEXT("%s: %.3f\n"), *Param.Key.ToString(), Param.Value);
            }
        }

        DrawDebugString(
            World,
            Loc + FVector(0.f, 0.f, 90.f),
            ParamStr,
            nullptr,
            ListenerColor,
            DrawDuration,
            true);
    }
}
