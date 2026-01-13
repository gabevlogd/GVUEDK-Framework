// Copyright Villains, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "EdMode.h"

class UStaticSpawnPoint;

class FStaticSpawnPointEdMode : public FEdMode
{
public:
	
	static const FEditorModeID EM_StaticSpawnPointEdModeId;

	FStaticSpawnPointEdMode();
	
	virtual ~FStaticSpawnPointEdMode() override;

	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;

	// the spawn point to edit
	UStaticSpawnPoint* TargetSpawnPoint = nullptr;

	// TargetSpawnPoint location being edited
	FVector CurrentPoint = FVector::ZeroVector;

};
