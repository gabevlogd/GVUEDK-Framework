#include "WorldTriggerContainer.h"

#include "MaterialDomain.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogWorldTriggerContainer, Log, All);

// ─── Lifecycle ───────────────────────────────────────────────────────────────

AWorldTriggerContainer::AWorldTriggerContainer()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
	TriggerVolume->SetGenerateOverlapEvents(true);
	RootComponent = TriggerVolume;


	
	
#if WITH_EDITOR
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UMaterial> TextMaterial;
		FConstructorStatics()
			: TextMaterial(TEXT("/Engine/EngineMaterials/DefaultTextMaterialTranslucent"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;
	
	ContainerLabelComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ContainerLabelComponent"));
	ContainerLabelComponent->SetWorldSize(100.f);
	ContainerLabelComponent->SetTextMaterial(ConstructorStatics.TextMaterial.Get());
	ContainerLabelComponent->SetHorizontalAlignment(EHTA_Center);
	ContainerLabelComponent->SetVerticalAlignment(EVRTA_TextCenter);
	ContainerLabelComponent->SetTextRenderColor(FColor::White);
	ContainerLabelComponent->SetUsingAbsoluteScale(true);
	ContainerLabelComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	ContainerLabelComponent->SetupAttachment(RootComponent);
	ContainerLabelComponent->SetText(ContainerLabel);
	ContainerLabelComponent->SetVisibility(true);
	ContainerLabelComponent->SetHiddenInGame(bHiddenInGame);
#endif
	
}

void AWorldTriggerContainer::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddUniqueDynamic(
		this, &AWorldTriggerContainer::OnOverlapBegin);

	TriggerVolume->OnComponentEndOverlap.AddUniqueDynamic(
		this, &AWorldTriggerContainer::OnOverlapEnd);
}

// ─── Overlap Callbacks ───────────────────────────────────────────────────────

void AWorldTriggerContainer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
											AActor* OtherActor,
											UPrimitiveComponent* OtherComp,
											int32 OtherBodyIndex,
											bool bFromSweep,
											const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor != this)
	{
		TryExecuteEnterAll(OtherActor);
	}
}

void AWorldTriggerContainer::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
										  AActor* OtherActor,
										  UPrimitiveComponent* OtherComp,
										  int32 OtherBodyIndex)
{
	if (IsValid(OtherActor) && OtherActor != this)
	{
		TryExecuteExitAll(OtherActor);
	}
}

// ─── Execution ───────────────────────────────────────────────────────────────

void AWorldTriggerContainer::TryExecuteEnterAll(AActor* Context)
{
	if (!bEnabled || !IsValid(Context))
	{
		return;
	}

	// Server-only in multiplayer
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	if (bDisableAfterFirstExecution && bHasExecutedOnce)
	{
		return;
	}

	UE_LOG(LogWorldTriggerContainer, Log,
		   TEXT("[%s] Enter ← %s"), *GetName(), *Context->GetName());

	for (UWorldTrigger* Trigger : Triggers)
	{
		if (!IsValid(Trigger))
		{
			UE_LOG(LogWorldTriggerContainer, Warning,
				   TEXT("[%s] Null trigger entry – skipping"), *GetName());
			continue;
		}

		Trigger->TryExecuteEnter(Context);
	}

	if (bDisableAfterFirstExecution)
	{
		bHasExecutedOnce = true;
		bEnabled = false;
	}
}

void AWorldTriggerContainer::TryExecuteExitAll(AActor* Context)
{
	if (!bEnabled || !IsValid(Context))
	{
		return;
	}

	if (GetNetMode() == NM_Client)
	{
		return;
	}

	UE_LOG(LogWorldTriggerContainer, Log,
		   TEXT("[%s] Exit ← %s"), *GetName(), *Context->GetName());

	for (UWorldTrigger* Trigger : Triggers)
	{
		if (!IsValid(Trigger))
		{
			continue;
		}

		Trigger->TryExecuteExit(Context);
	}
}

// ─── Editor ──────────────────────────────────────────────────────────────────

#if WITH_EDITOR

void AWorldTriggerContainer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (UWorldTrigger* Trigger : Triggers)
	{
		if (!IsValid(Trigger)) continue;

		auto ValidateArray = [&](const TArray<UTriggerCondition*>& Conditions,
								 const TArray<UTriggerAction*>& Actions,
								 const FString& EventLabel)
		{
			for (const UTriggerCondition* Cond : Conditions)
			{
				if (!IsValid(Cond)) continue;
				const FString Warning = Cond->GetEditorValidationWarning();
				if (!Warning.IsEmpty())
				{
					UE_LOG(LogWorldTriggerContainer, Warning,
						   TEXT("[%s][%s] Condition '%s': %s"),
						   *Trigger->TriggerLabel, *EventLabel,
						   *Cond->GetName(), *Warning);
				}
			}

			for (const UTriggerAction* Action : Actions)
			{
				if (!IsValid(Action)) continue;
				const FString Warning = Action->GetEditorValidationWarning();
				if (!Warning.IsEmpty())
				{
					UE_LOG(LogWorldTriggerContainer, Warning,
						   TEXT("[%s][%s] Action '%s': %s"),
						   *Trigger->TriggerLabel, *EventLabel,
						   *Action->GetName(), *Warning);
				}
			}
		};

		ValidateArray(Trigger->EnterConditions, Trigger->EnterActions, TEXT("Enter"));
		ValidateArray(Trigger->ExitConditions,  Trigger->ExitActions,  TEXT("Exit"));
	}

	if (IsValid(ContainerLabelComponent))
	{
		ContainerLabelComponent->SetText(ContainerLabel);
		ContainerLabelComponent->SetHiddenInGame(bHiddenInGame);
	}
	TriggerVolume->ShapeColor = TriggerBoxColor;
	TriggerVolume->SetHiddenInGame(bHiddenInGame);
}

void AWorldTriggerContainer::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);

	for (UWorldTrigger* Trigger : Triggers)
	{
		if (!IsValid(Trigger)) continue;

		if (Trigger->GetOuter() != this)
		{
			UE_LOG(LogWorldTriggerContainer, Warning,
				   TEXT("[%s] PostDuplicate: Trigger '%s' has unexpected Outer."),
				   *GetName(), *Trigger->GetName());
		}
	}
}

#endif
