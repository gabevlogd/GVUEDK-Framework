// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Subsystems/FMODSoundtrackSubsystem.h"
#include "FMODSoundtrackHelper.generated.h"

UCLASS()
class FMODEVENTSMANAGERSYSTEM_API AFMODSoundtrackHelper : public AActor
{
	GENERATED_BODY()

public:
	
	AFMODSoundtrackHelper();

protected:
	
	virtual void BeginPlay() override;

	void Initialize();

	bool Initialized() const { return SoundtrackSubsystem != nullptr; }

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	UFMODSoundtrackSubsystem* SoundtrackSubsystem = nullptr;
};
