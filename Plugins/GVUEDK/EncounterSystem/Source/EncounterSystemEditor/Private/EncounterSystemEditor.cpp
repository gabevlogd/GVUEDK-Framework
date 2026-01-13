#include "EncounterSystemEditor.h"

#include "EditorModeRegistry.h"
#include "SWaveGraphPanel.h"
#include "Data/CostBasedGroupData.h"
#include "EditorModes/StaticSpawnPointEdMode.h"
#include "DetailCustomizations/EnemyGroupDataCustomization.h"
#include "Data/EnemyGroupData.h"

#define LOCTEXT_NAMESPACE "FEncounterSystemEditorModule"

void FEncounterSystemEditorModule::StartupModule()
{
	// Register the custom editor mode for static spawn points
	FEditorModeRegistry::Get().RegisterMode<FStaticSpawnPointEdMode>(
		 FStaticSpawnPointEdMode::EM_StaticSpawnPointEdModeId,
		 FText::FromString("Static Spawn Point Editor"),
		 FSlateIcon(),
		 false
	 );
	
	// Register the customization for UEnemyGroupData, in other words: link the data asset to the custom detail panel
	FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditor.RegisterCustomClassLayout(
		UEnemyGroupData::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FEnemyGroupDataCustomization::MakeInstance)
	);
	PropertyEditor.RegisterCustomClassLayout(
		UCostBasedGroupData::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FEnemyGroupDataCustomization::MakeInstance)
	);

	// Register the Wave Debugger tab
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		"WavePropertyIncrementsGraphs",
		FOnSpawnTab::CreateRaw(this, &FEncounterSystemEditorModule::OnSpawnPluginTab)
	)
	.SetDisplayName(NSLOCTEXT("WavePropertyIncrementsGraphs", "TabTitle", "Wave Property Increments Graphs"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);

	// Add the Wave Debugger entry to the Window menu
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		FToolMenuSection& Section = Menu->AddSection("WavePropertyIncrementsGraphs");
		Section.AddMenuEntry( "OpenWavePropertyIncrementsGraphs",
			FText::FromString("Wave Property Increments Graphs"),
			FText::FromString("Open Wave Property Increments Graphs Tool"),
			FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]()
			{
				FGlobalTabmanager::Get()->TryInvokeTab(FTabId("WavePropertyIncrementsGraphs"));
			})));
	}));
}

void FEncounterSystemEditorModule::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(FStaticSpawnPointEdMode::EM_StaticSpawnPointEdModeId);
	
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditor.UnregisterCustomClassLayout(UEnemyGroupData::StaticClass()->GetFName());
		PropertyEditor.UnregisterCustomClassLayout(UCostBasedGroupData::StaticClass()->GetFName());
	}
}

TSharedRef<SDockTab> FEncounterSystemEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		[
			SNew(SWaveGraphPanel)
		];
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FEncounterSystemEditorModule, EncounterSystemEditor)