// Copyright Villains, Inc. All Rights Reserved.


#include "Widgets/CompassWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Intrefaces/CompassTarget.h"


void UCompassWidget::AddTarget(const FName& Name, UObject* TargetIcon)
{
	if (!TargetIcon) return;
	if (!WidgetTree) return;
	if (CompassTargets.Contains(Name)) return;
	
	UImage* NewImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), Name);

	CompassTargets.Add(Name, NewImage);
	
	FSlateBrush Brush;
	Brush.SetResourceObject(TargetIcon); 
	Brush.ImageSize = FVector2D(64, 64); 
	NewImage->SetBrush(Brush);
	
	if (UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(GetRootWidget()))
	{
		UCanvasPanelSlot* NewSlot = RootCanvas->AddChildToCanvas(NewImage);
		NewSlot->SetPosition(FVector2D(0, 0));
		NewSlot->SetAnchors(FAnchors(0.5f, 0.f, 0.5f, 0.f));  
		//NewSlot->SetAlignment(FVector2D(0.f, 0.0f));            
		NewSlot->SetPosition(FVector2D(0, TargetIconsVerticalOffset));
		NewSlot->SetSize(DefaultTargetIconsSize);
	}

	
}

void UCompassWidget::RemoveTarget(const FName& Name)
{
	if (!CompassTargets.Contains(Name)) return;
	
	CompassTargets[Name]->RemoveFromParent();
	CompassTargets.Remove(Name);
}

void UCompassWidget::UpdateTargetIconPosition(const FName& Name, const float Position, const bool bHideIconIfOutOfSight)
{
	if (!CompassTargets.Contains(Name)) return;
	
	UImage* CompassMarker = CompassTargets[Name];

	if (bHideIconIfOutOfSight)
	{
		if (FMath::IsNearlyEqual(Position, 1.f, 0.01f) || FMath::IsNearlyEqual(Position, -1.f, 0.01f))
		{
			CompassMarker->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (CompassMarker->GetVisibility() == ESlateVisibility::Hidden)
		{
			CompassMarker->SetVisibility(ESlateVisibility::Visible);
		}
	}

	const FGeometry Geometry = GetCachedGeometry();
	const float WidgetWidth = Geometry.GetLocalSize().X * 0.5f;
	
	if (UCanvasPanelSlot* MarkerSlot = Cast<UCanvasPanelSlot>(CompassMarker->Slot))
	{
		const float MarkerWidth = MarkerSlot->GetSize().X;
		const float X = Position * WidgetWidth - (MarkerWidth * MarkerSlot->GetAlignment().X);

		MarkerSlot->SetPosition(FVector2D(X, MarkerSlot->GetPosition().Y));
	}
}
