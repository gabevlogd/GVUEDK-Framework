

#include "DetailCustomizations/EnemyGroupDataCustomization.h"
#include "Data/EnemyGroupData.h"
#include "SpawnSystem/SpawnMethods/StaticSpawnPoint.h"
#include "EditorModes/StaticSpawnPointEdMode.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorModeManager.h"
#include "Data/CostBasedGroupData.h"

TSharedRef<IDetailCustomization> FEnemyGroupDataCustomization::MakeInstance()
{
    return MakeShareable(new FEnemyGroupDataCustomization());
}

void FEnemyGroupDataCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    // Take the object being customized, which should be a UWaveData.
    // The taken object is defined in the Module startup when registering the customization.
    TArray<TWeakObjectPtr<UObject>> Objects;
    DetailBuilder.GetObjectsBeingCustomized(Objects);

    if (Objects.Num() == 0)
        return;

    int32 TotalGroupNum;
    TArray<FEnemyGroup> EnemyGroups;
    
    if (const UEnemyGroupData* EnemyGroupData = Cast<UEnemyGroupData>(Objects[0].Get()))
    {
        TotalGroupNum = EnemyGroupData->EnemyGroups.Num();
        EnemyGroups = EnemyGroupData->EnemyGroups;
    }
    else if (const UCostBasedGroupData* CostBasedGroupData = Cast<UCostBasedGroupData>(Objects[0].Get()))
    {
        TotalGroupNum = CostBasedGroupData->EnemyGroups.Num();
        for (const FCostBasedGroup& CostBasedGroup : CostBasedGroupData->EnemyGroups)
        {
            EnemyGroups.Add(CostBasedGroup);
        }
    }
    else return;

    IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Spawn Points");

    // Iterate through enemy groups 
    for (int32 GroupIndex = 0; GroupIndex < TotalGroupNum; ++GroupIndex)
    {
        const FEnemyGroup& Group = EnemyGroups[GroupIndex];

        if (!Group.SpawnMethod)
            continue;

        // if is a UStaticSpawnPoint
        if (UStaticSpawnPoint* StaticSpawn = Cast<UStaticSpawnPoint>(Group.SpawnMethod))
        {
            FText Label = FText::Format(
                FText::FromString("Group {0} Spawn Location"),
                FText::AsNumber(GroupIndex)
            );

            Category.AddCustomRow(Label)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth()
                [
                    SNew(STextBlock).Text(Label)
                ]
                + SHorizontalBox::Slot().AutoWidth()
                [
                    SNew(SButton)
                    .Text(FText::FromString("Edit in Viewport"))
                    .OnClicked_Lambda([StaticSpawn]()
                    {
                        GLevelEditorModeTools().ActivateMode(FStaticSpawnPointEdMode::EM_StaticSpawnPointEdModeId);

                        FStaticSpawnPointEdMode* Mode = (FStaticSpawnPointEdMode*)GLevelEditorModeTools().GetActiveMode(FStaticSpawnPointEdMode::EM_StaticSpawnPointEdModeId);

                        Mode->TargetSpawnPoint = StaticSpawn;
                        Mode->CurrentPoint = StaticSpawn->GetSpawnPointLocation();

                        return FReply::Handled();
                    })
                ]
            ];
        }
    }
}
