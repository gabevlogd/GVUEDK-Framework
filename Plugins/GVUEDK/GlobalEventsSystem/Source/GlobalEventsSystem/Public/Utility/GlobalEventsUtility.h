// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GlobalEventsSubsystem.h"
#include "GlobalEventsUtility.generated.h"

class FGlobalEventWithObject;
class UGlobalEventsSubsystem;

UCLASS()
class GLOBALEVENTSSYSTEM_API UGlobalEventsUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static void Initialize(UGlobalEventsSubsystem* GlobalEvents);

	UFUNCTION(BlueprintCallable, Category = "GlobalEvents")
	static void BindEvent(const FGameplayTag EventTag, const FGloablEvent& Event);

	UFUNCTION(BlueprintCallable, Category = "GlobalEvents", meta = (DisplayName = "Bind Event"))
	static void BindObjectEvent(const FGameplayTag EventTag, const FGlobalEventWithObject& Event);

	UFUNCTION(BlueprintCallable, Category = "GlobalEvents")
	static void UnbindEvent(const FGameplayTag EventTag, const FGloablEvent& Event);

	UFUNCTION(BlueprintCallable, Category = "GlobalEvents", meta = (DisplayName = "Unbind Event"))
	static void UnbindObjectEvent(const FGameplayTag EventTag, const FGlobalEventWithObject& Event);
	
	UFUNCTION(BlueprintCallable, Category = "GlobalEvents")
	static void CallEvent(const FGameplayTag EventTag);

	UFUNCTION(BlueprintCallable, Category = "GlobalEvents", meta = (DisplayName = "Call Event"))
	static void CallObjectEvent(const FGameplayTag EventTag, UObject* Object);

private:

	static UGlobalEventsSubsystem* GlobalEventsSubsystem;

	
};
