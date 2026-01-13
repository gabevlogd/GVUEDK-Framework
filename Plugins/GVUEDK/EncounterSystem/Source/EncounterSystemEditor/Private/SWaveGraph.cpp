#include "SWaveGraph.h"

#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Fonts/SlateFontInfo.h"

int32 SWaveGraph::OnPaint(
    const FPaintArgs& Args,
    const FGeometry& Geometry,
    const FSlateRect& CullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& WidgetStyle,
    bool bParentEnabled
) const
{
    if (Curves.Num() == 0)
        return LayerId;

    int32 Layer = LayerId;

    const FVector2f Size(Geometry.GetLocalSize());
    constexpr float Padding = 40.f;

    const FVector2f Origin(Padding, Size.Y - Padding);
    const FVector2f XEnd(Size.X - Padding, Size.Y - Padding);
    const FVector2f YEnd(Padding, Padding);

    const float GraphWidth  = Size.X - Padding * 2.f;
    const float GraphHeight = Size.Y - Padding * 2.f;

    const float MaxValue = GetMaxValue();

    const FPaintGeometry RootPG = Geometry.ToPaintGeometry(Size, FSlateLayoutTransform());

    // Assi
    FSlateDrawElement::MakeLines(
        OutDrawElements,
        Layer++,
        RootPG,
        { Origin, XEnd, Origin, YEnd },
        ESlateDrawEffect::None,
        FLinearColor::White,
        true,
        1.5f
    );

    // Asse X label
    {
        const FVector2f Pos(Size.X*0.5f, Size.Y - 24.f);
        FSlateDrawElement::MakeText(
            OutDrawElements,
            Layer++,
            Geometry.ToPaintGeometry(FVector2f::ZeroVector, FSlateLayoutTransform(Pos)),
            FText::FromString(TEXT("Wave Index")),
            FCoreStyle::GetDefaultFontStyle("Regular",10),
            ESlateDrawEffect::None,
            FLinearColor::White
        );
    }

    // Asse Y label
    {
        const FVector2f Pos(6.f, Size.Y*0.5f);
        FSlateDrawElement::MakeText(
            OutDrawElements,
            Layer++,
            Geometry.ToPaintGeometry(FVector2f::ZeroVector, FSlateLayoutTransform(Pos)),
            FText::FromString(TEXT("Value")),
            FCoreStyle::GetDefaultFontStyle("Regular",10),
            ESlateDrawEffect::None,
            FLinearColor::White
        );
    }

    // Curves
    for (const FWaveGraphCurve& Curve : Curves)
    {
        int32 Count = Curve.Values.Num();
        if (Count < 2) continue;

        for (int32 i = 0; i < Count - 1; ++i)
        {
            const FVector2f A(
                Origin.X + (float)i/(Count-1)*GraphWidth,
                Origin.Y - (Curve.Values[i]/MaxValue)*GraphHeight
            );
            const FVector2f B(
                Origin.X + (float)(i+1)/(Count-1)*GraphWidth,
                Origin.Y - (Curve.Values[i+1]/MaxValue)*GraphHeight
            );

            FSlateDrawElement::MakeLines(
                OutDrawElements,
                Layer,
                RootPG,
                { A, B },
                ESlateDrawEffect::None,
                Curve.Color,
                true,
                2.f
            );
        }
    }

    // Legenda
    float LegendY = 10.f;
    for (const FWaveGraphCurve& Curve : Curves)
    {
        // Box colore
        FVector2f BoxPos(Size.X - 130.f, LegendY);
        FVector2f BoxSize(10.f, 10.f);
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            Layer,
            Geometry.ToPaintGeometry(BoxSize, FSlateLayoutTransform(BoxPos)),
            FCoreStyle::Get().GetBrush("WhiteBrush"),
            ESlateDrawEffect::None,
            Curve.Color
        );

        // Label
        FVector2f TextPos(Size.X - 115.f, LegendY - 2.f);
        FSlateDrawElement::MakeText(
            OutDrawElements,
            Layer,
            Geometry.ToPaintGeometry(FVector2f::ZeroVector, FSlateLayoutTransform(TextPos)),
            FText::FromString(Curve.Label),
            FCoreStyle::GetDefaultFontStyle("Regular",9),
            ESlateDrawEffect::None,
            FLinearColor::White
        );

        LegendY += 14.f;
    }

    return Layer;
}
