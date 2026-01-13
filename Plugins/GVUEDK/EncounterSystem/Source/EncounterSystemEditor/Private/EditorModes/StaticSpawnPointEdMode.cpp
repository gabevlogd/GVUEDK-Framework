// Copyright Villains, Inc. All Rights Reserved.


#include "EditorModes/StaticSpawnPointEdMode.h"
#include "SpawnSystem/SpawnMethods/StaticSpawnPoint.h"
#include "EditorModeManager.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Engine/Engine.h"

const FEditorModeID FStaticSpawnPointEdMode::EM_StaticSpawnPointEdModeId =
    TEXT("EM_StaticSpawnPointEdMode");

FStaticSpawnPointEdMode::FStaticSpawnPointEdMode() {}
FStaticSpawnPointEdMode::~FStaticSpawnPointEdMode() {}

bool FStaticSpawnPointEdMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
    // ESC → Deactivate mode
    if (Key == EKeys::Escape && Event == IE_Pressed)
    {
        GLevelEditorModeTools().DeactivateMode(GetID());
        return true;
    }
    return FEdMode::InputKey(ViewportClient, Viewport, Key, Event);
}

void FStaticSpawnPointEdMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
    FEdMode::Render(View, Viewport, PDI);

    if (!Viewport) return;

    // Draw tooltip
    FCanvas* Canvas = Viewport->GetDebugCanvas();
    if (Canvas)
    {
        // Draw usage instruction
        const FString Tooltip = TEXT("Shift + LMB to Move Spawn Point");
        FCanvasTextItem TextItem(
            FVector2D(50.f, 50.f),
            FText::FromString(Tooltip),
            GEngine->GetLargeFont(),
            FLinearColor::Red
        );
        TextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(TextItem);

        // Draw exit instruction
        const FString ToolTip2 = TEXT("ESC to Exit Edit Mode (focus on viewport is required)");
        FCanvasTextItem TextItem2(
            FVector2D(50.f, 80.f),
            FText::FromString(ToolTip2),
            GEngine->GetLargeFont(),
            FLinearColor::Red
        );
        TextItem2.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(TextItem2);
    }

    // Draw gizmo for current target
    if (TargetSpawnPoint)
    {
        TargetSpawnPoint->DrawGizmo(PDI);
    }
}

bool FStaticSpawnPointEdMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
    // Require SHIFT key
    if (!FSlateApplication::Get().GetModifierKeys().IsShiftDown())
    {
        return true; // ignore click
    }

    FHitResult Hit;
    const FVector Start = Click.GetOrigin();
    const FVector End = Start + Click.GetDirection() * 100000.f;

    if (InViewportClient->GetWorld()->LineTraceSingleByChannel(
        Hit, Start, End, ECC_Visibility))
    {
        CurrentPoint = Hit.Location;

        if (TargetSpawnPoint)
        {
            TargetSpawnPoint->Modify();
            TargetSpawnPoint->SetSpawnPointLocation(CurrentPoint);
        }
    }

    return true;
}
