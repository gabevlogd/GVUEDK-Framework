// Copyright Villains, Inc. All Rights Reserved.


#include "Actors/FMODSoundtrackHelper.h"


// Sets default values
AFMODSoundtrackHelper::AFMODSoundtrackHelper()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFMODSoundtrackHelper::BeginPlay()
{
	Initialize();
	Super::BeginPlay();
	
}

void AFMODSoundtrackHelper::Initialize()
{
	SoundtrackSubsystem = GetGameInstance()->GetSubsystem<UFMODSoundtrackSubsystem>();
}

