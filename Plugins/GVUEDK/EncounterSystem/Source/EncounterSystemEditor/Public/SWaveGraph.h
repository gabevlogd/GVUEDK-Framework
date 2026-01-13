#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "SWaveGraphPanel.h"

class SWaveGraph : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWaveGraph) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs) {}

	void SetCurves(const TArray<FWaveGraphCurve>& InCurves) { Curves = InCurves; }

	virtual int32 OnPaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
	) const override;

private:
	TArray<FWaveGraphCurve> Curves;

	float GetMaxValue() const
	{
		float MaxValue = 0.f;
		for (const auto& Curve : Curves)
		{
			for (float V : Curve.Values)
				MaxValue = FMath::Max(MaxValue, V);
		}
		return FMath::Max(MaxValue, 1.f);
	}
};

