// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalEventsSubsystem.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogGlobalEventsSystem, All, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGlobalEventSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGlobalEventSignatureWithObject, UObject*, Object);

DECLARE_DYNAMIC_DELEGATE(FGloablEvent);
DECLARE_DYNAMIC_DELEGATE_OneParam(FGlobalEventWithObject, UObject*, Object);

UCLASS()
class GLOBALEVENTSSYSTEM_API UGlobalEventsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	void BindEvent(const FGameplayTag& EventTag, const FGloablEvent& Event);

	void BindObjectEvent(const FGameplayTag& EventTag, const FGlobalEventWithObject& Event);

	void UnbindEvent(const FGameplayTag& EventTag, const FGloablEvent& Event);

	void UnbindObjectEvent(const FGameplayTag& EventTag, const FGlobalEventWithObject& Event);

	void CallEvent(const FGameplayTag& EventTag);

	void CallObjectEvent(const FGameplayTag& EventTag, UObject* Object);

private:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	
	TMap<FGameplayTag, FGlobalEventSignature> EventMap;
	TMap<FGameplayTag, FGlobalEventSignatureWithObject> ObjectEventMap;
};
