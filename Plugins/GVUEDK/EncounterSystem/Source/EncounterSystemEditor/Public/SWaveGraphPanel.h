#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UWaveData;

struct FWaveGraphCurve
{
	FString Label;
	FLinearColor Color;
	TArray<float> Values;
};

// Forward declaration
class SWaveGraph;

class SWaveGraphPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWaveGraphPanel) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	// ---- UI ----
	UWaveData* SelectedWaveData = nullptr;
	TSharedPtr<SWaveGraph> GraphWidget;

	// ---- Data ----
	TArray<FWaveGraphCurve> Curves;

	void Recalculate();
};


