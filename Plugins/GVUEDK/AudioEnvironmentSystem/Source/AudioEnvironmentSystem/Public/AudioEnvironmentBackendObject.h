// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AudioEnvironmentSystemModule.h"
#include "IAudioEnvironmentBackend.h"
#include "UObject/Object.h"
#include "AudioEnvironmentBackendObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AUDIOENVIRONMENTSYSTEM_API UAudioEnvironmentBackendObject : public UObject, public IAudioEnvironmentBackend
{
	GENERATED_BODY()

public:

	// --------------------------------------------------------
	//  IAudioEnvironmentBackend
	// --------------------------------------------------------

	virtual void OnListenerEnvironmentChanged(
		const FGuid& InListenerID,
		const FAudioEnvironmentListenerState& InPrevState,
		const FAudioEnvironmentListenerState& InNewState) override;

	virtual void OnListenerEnvironmentTick(
		const FGuid& InListenerID,
		const FAudioEnvironmentListenerState& InCurrentState) override;

	virtual void Initialize() override;

	virtual void Shutdown() override;

protected:

	// --------------------------------------------------------
	//  Blueprint events (call native implementations)
	// --------------------------------------------------------

	UFUNCTION(BlueprintImplementableEvent, Category = "Audio|Environment", meta = (DisplayName = "OnListenerEnvironmentChanged"))
	void BP_OnListenerEnvironmentChanged(
		const FGuid& InListenerID,
		const FAudioEnvironmentListenerState& InPrevState,
		const FAudioEnvironmentListenerState& InNewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "Audio|Environment", meta = (DisplayName = "OnListenerEnvironmentTick"))
	void BP_OnListenerEnvironmentTick(
		const FGuid& InListenerID,
		const FAudioEnvironmentListenerState& InCurrentState);

	UFUNCTION(BlueprintImplementableEvent, Category = "Audio|Environment", meta = (DisplayName = "Initialize"))
	void BP_Initialize();

	UFUNCTION(BlueprintImplementableEvent, Category = "Audio|Environment", meta = (DisplayName = "Shutdown"))
	void BP_Shutdown();

private:
	
	// --------------------------------------------------------
	//  UObject overrides
	// --------------------------------------------------------

#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override { return true; }
#endif

	virtual UWorld* GetWorld() const override
	{
		if (const UObject* Outer = GetOuter())
		{
			return Outer->GetWorld();
		}

		UE_LOG(LogAudioEnvironment, Error,
			TEXT("%s: GetWorld() failed because this object has no outer. Returning nullptr."),
			*GetName());
		return nullptr;
	}
};
