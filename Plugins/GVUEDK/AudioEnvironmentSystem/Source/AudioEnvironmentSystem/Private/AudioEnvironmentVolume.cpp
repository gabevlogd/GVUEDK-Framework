// Copyright (c) 2024. All Rights Reserved.

#include "AudioEnvironmentVolume.h"

#include "AudioEnvironmentSystemModule.h"
#include "AudioEnvironmentSubsystem.h"
#include "AudioEnvironmentPreset.h"
#include "AudioEnvironmentBlender.h"

#include "Components/BoxComponent.h"
#include "Curves/CurveFloat.h"
#include "Engine/World.h"

// ============================================================
//  Constructor
// ============================================================

AAudioEnvironmentVolume::AAudioEnvironmentVolume()
{
    // Volumes only need spatial data — no per-frame Tick.
    PrimaryActorTick.bCanEverTick = false;

    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    SetRootComponent(BoxComponent);

    // Sensible editor defaults
    BoxComponent->SetBoxExtent(FVector(500.f, 500.f, 300.f));
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BoxComponent->SetGenerateOverlapEvents(false);   // We do NOT use overlap events.
    BoxComponent->SetHiddenInGame(true);             // Invisible at runtime; debug draws handle visualization.
    BoxComponent->ShapeColor = FColor(0, 200, 255);  // Distinct cyan tint in the editor.
}

// ============================================================
//  AActor overrides
// ============================================================

void AAudioEnvironmentVolume::BeginPlay()
{
    Super::BeginPlay();

    SyncLayerTagFromPreset();

    if (UWorld* World = GetWorld())
    {
        if (UAudioEnvironmentSubsystem* Subsystem = World->GetSubsystem<UAudioEnvironmentSubsystem>())
        {
            Subsystem->RegisterVolume(this);
        }
    }
}

void AAudioEnvironmentVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        if (UAudioEnvironmentSubsystem* Subsystem = World->GetSubsystem<UAudioEnvironmentSubsystem>())
        {
            Subsystem->UnregisterVolume(this);
        }
    }

    Super::EndPlay(EndPlayReason);
}

// ============================================================
//  IAudioEnvironmentShape — Box implementation
// ============================================================

bool AAudioEnvironmentVolume::IsLocationInside(const FVector& InWorldLocation) const
{
    if (!BoxComponent)
    {
        return false;
    }

    const FVector LocalPoint = BoxComponent->GetComponentTransform()
        .InverseTransformPosition(InWorldLocation);
    const FVector Extent = BoxComponent->GetScaledBoxExtent();

    return FMath::Abs(LocalPoint.X) <= Extent.X
        && FMath::Abs(LocalPoint.Y) <= Extent.Y
        && FMath::Abs(LocalPoint.Z) <= Extent.Z;
}

float AAudioEnvironmentVolume::GetNormalizedDepth(
    const FVector& InWorldLocation, float InBlendDistance) const
{
    if (!BoxComponent)
    {
        return 0.f;
    }

    // Transform location into the component's local space.
    const FVector LocalPoint = BoxComponent->GetComponentTransform()
        .InverseTransformPosition(InWorldLocation);
    const FVector Extent = BoxComponent->GetScaledBoxExtent();

    // Delta[i] = how far we are from the face along each axis.
    // Negative on any axis means the point is outside the box on that axis.
    const FVector Delta = Extent - LocalPoint.GetAbs();

    if (Delta.X < 0.f || Delta.Y < 0.f || Delta.Z < 0.f)
    {
        return 0.f;   // Outside the volume.
    }

    // The minimum delta is the distance to the nearest face (depth inside the volume).
    const float MinDepthFromFace = FMath::Min3(Delta.X, Delta.Y, Delta.Z);

    if (InBlendDistance <= KINDA_SMALL_NUMBER)
    {
        return 1.f;   // No blend zone; instantly at full weight anywhere inside.
    }

    return FMath::Clamp(MinDepthFromFace / InBlendDistance, 0.f, 1.f);
}

FVector AAudioEnvironmentVolume::GetClosestSurfacePoint(const FVector& InWorldLocation) const
{
    if (!BoxComponent)
    {
        return GetActorLocation();
    }

    const FTransform& Transform = BoxComponent->GetComponentTransform();
    const FVector LocalPoint = Transform.InverseTransformPosition(InWorldLocation);
    const FVector Extent = BoxComponent->GetScaledBoxExtent();

    // Clamp to the box surface in local space.
    const FVector ClampedLocal = LocalPoint.BoundToBox(-Extent, Extent);
    return Transform.TransformPosition(ClampedLocal);
}

FBoxSphereBounds AAudioEnvironmentVolume::GetShapeBounds() const
{
    if (BoxComponent)
    {
        return BoxComponent->CalcBounds(BoxComponent->GetComponentTransform());
    }
    return FBoxSphereBounds(GetActorLocation(), FVector(100.f), 100.f);
}

// ============================================================
//  Evaluation API
// ============================================================

float AAudioEnvironmentVolume::EvaluateWeightAtLocation(const FVector& InWorldLocation) const
{
    if (!bEnabled)
    {
        return 0.f;
    }

    const float NormalizedDepth = GetNormalizedDepth(InWorldLocation, BlendDistance);

    if (NormalizedDepth <= KINDA_SMALL_NUMBER)
    {
        return 0.f;
    }

    return ApplyBlendCurve(NormalizedDepth);
}

FAudioEnvironmentParameters AAudioEnvironmentVolume::GetParameters() const
{
    if (Preset)
    {
        return Preset->GetParameters();
    }
    return FAudioEnvironmentParameters{};
}

// ============================================================
//  Internal helpers
// ============================================================

float AAudioEnvironmentVolume::ApplyBlendCurve(float InNormalizedDepth) const
{
    switch (BlendMode)
    {
    case EAudioEnvironmentBlendMode::Linear:
        return FMath::Clamp(InNormalizedDepth, 0.f, 1.f);

    case EAudioEnvironmentBlendMode::Smoothstep:
        return UAudioEnvironmentBlender::Smoothstep(InNormalizedDepth);

    case EAudioEnvironmentBlendMode::Curve:
        if (BlendCurve)
        {
            return FMath::Clamp(BlendCurve->GetFloatValue(InNormalizedDepth), 0.f, 1.f);
        }
        // Graceful fallback if the curve asset is not assigned.
        UE_LOG(LogAudioEnvironment, Verbose,
            TEXT("Volume '%s': BlendMode=Curve but no BlendCurve assigned. Falling back to Smoothstep."),
            *GetName());
        return UAudioEnvironmentBlender::Smoothstep(InNormalizedDepth);

    default:
        return FMath::Clamp(InNormalizedDepth, 0.f, 1.f);
    }
}

void AAudioEnvironmentVolume::SyncLayerTagFromPreset()
{
    if (Preset && LayerTag.IsNone())
    {
        LayerTag = Preset->GetLayerTag();
    }
}

// ============================================================
//  Editor support
// ============================================================

#if WITH_EDITOR

void AAudioEnvironmentVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName ChangedProp = PropertyChangedEvent.GetPropertyName();

    // Auto-sync layer tag when the preset changes
    if (ChangedProp == GET_MEMBER_NAME_CHECKED(AAudioEnvironmentVolume, Preset))
    {
        // Reset LayerTag so SyncLayerTagFromPreset can update it.
        LayerTag = NAME_None;
        SyncLayerTagFromPreset();
    }
}

void AAudioEnvironmentVolume::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    SyncLayerTagFromPreset();
}

#endif // WITH_EDITOR
