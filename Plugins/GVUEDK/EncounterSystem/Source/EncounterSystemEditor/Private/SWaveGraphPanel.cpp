#include "SWaveGraphPanel.h"
#include "PropertyCustomizationHelpers.h"
#include "SWaveGraph.h"
#include "Data/WaveData.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"
#include "Fonts/SlateFontInfo.h"
#include "WaveSystem/WaveExecutionModes/Base/WaveExecutionMode.h"
#include "WaveSystem/WaveExecutionModes/EndlessMode/EndlessMode.h"

// ---------------------------
// Construct
// ---------------------------

void SWaveGraphPanel::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)

		// Titolo
		+ SVerticalBox::Slot().AutoHeight().Padding(6)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Wave Property Increments Graphs")))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
		]

		// Picker + bottone
		+ SVerticalBox::Slot().AutoHeight().Padding(6)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot().FillWidth(1.f)
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(UWaveData::StaticClass()) // Only allow selecting WaveData assets
				.AllowClear(true) // Allow clearing the selection
				.ObjectPath_Lambda([this]() -> FString // Get the path of the selected asset for display in the picker
				{
					return (SelectedWaveData ? SelectedWaveData->GetPathName() : FString());
				})
				.OnObjectChanged_Lambda([this](const FAssetData& AssetData)
				{
					SelectedWaveData = Cast<UWaveData>(AssetData.GetAsset());
				})
			]

			+ SHorizontalBox::Slot().AutoWidth().Padding(4, 0)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Recalculate")))
				.OnClicked_Lambda([this]()
				{
					Recalculate();
					if (GraphWidget.IsValid())
					{
						GraphWidget->SetCurves(Curves);
					}
					return FReply::Handled();
				})
			]
		]

		// Grafico
		+ SVerticalBox::Slot().FillHeight(1.f).Padding(6)
		[
			SAssignNew(GraphWidget, SWaveGraph)
		]
	];
}

// ---------------------------
// Recalculate (dummy simulation)
// ---------------------------

void SWaveGraphPanel::Recalculate()
{
	Curves.Empty();

	if (!IsValid(SelectedWaveData))
		return;

	if (SelectedWaveData->Waves.Num() == 0)
		return;

	if (!IsValid(SelectedWaveData->Waves[0].WaveExecutionMode))
		return;

	const UEndlessMode* EndlessMode = Cast<UEndlessMode>(SelectedWaveData->Waves[0].WaveExecutionMode);
	if (!IsValid(EndlessMode))
		return;

	constexpr int32 NumWaves = 50;
	
	FWaveGraphCurve BudgetCurve;
	BudgetCurve.Label = TEXT("Total Budget");
	BudgetCurve.Color = FLinearColor::Green;
	EndlessMode->SimulateBudgetGeneration(NumWaves, BudgetCurve.Values);
	Curves.Add(BudgetCurve);
}

// constexpr int32 NumWaves = 50;
//
// FWaveGraphCurve BudgetCurve;
// BudgetCurve.Label = TEXT("Total Budget");
// BudgetCurve.Color = FLinearColor::Green;
//
// FWaveGraphCurve CostCurve;
// CostCurve.Label = TEXT("Total Cost");
// CostCurve.Color = FLinearColor::Red;
//
// for (int32 Wave = 0; Wave < NumWaves; ++Wave)
// {
// 	float Budget = 10.f * FMath::Pow(1.15f, Wave);
// 	float Cost = 6.f * FMath::Loge(Wave + 1.f) * 4.f;
//
// 	BudgetCurve.Values.Add(Budget);
// 	CostCurve.Values.Add(Cost);
// }
//
// Curves.Add(BudgetCurve);
// Curves.Add(CostCurve);
