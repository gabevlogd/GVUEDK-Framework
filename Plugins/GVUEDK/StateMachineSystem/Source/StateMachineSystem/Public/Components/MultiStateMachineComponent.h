// Copyright (c) 2025, Gabevlogd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StateMachineComponent.h"
#include "Components/ActorComponent.h"
#include "Data/StateMachineConfig.h"
#include "MultiStateMachineComponent.generated.h"

class UStateBase;
struct FInputActionValue;
DEFINE_LOG_CATEGORY_STATIC(LogMultiStateMachine, All, All);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStateChanged, FGameplayTag, StateMachineTag, FGameplayTag, PreviousState, FGameplayTag, NewState);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STATEMACHINESYSTEM_API UMultiStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category="Multi-State Machine")
	FOnStateChanged OnStateChanged;

	UPROPERTY(BlueprintAssignable, Category="Multi-State Machine")
	FOnReplicatedDataChanged OnReplicatedDataChanged;

private:

	friend class UStateMachineComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Multi-State Machine", meta = (AllowPrivateAccess = "true"))
	TArray<UStateMachineConfig*> StateMachines;

	UPROPERTY(BlueprintReadOnly, Category = "Multi-State Machine", meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, UStateMachineComponent*> StateMachinesMap;
	
	bool bInitialized;
	bool bPaused;

public:
	UMultiStateMachineComponent();

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	/**
     *  Change the state of the state machine with the given tag
     */
	UFUNCTION(BlueprintCallable, Category = "Multi-State Machine")
	void ChangeState(const FGameplayTag NextState, const FGameplayTag RelativeStateMachine);

	/**
	 *  Returns the state machine component with the given tag
	 *  @param InputActionTag - The tag of the input action to handle
	 *  @param Value - The value of the input action
	 *  @param RelativeStateMachine - The tag of the state machine which has to handle the input
	 */
	UFUNCTION(BlueprintCallable, Category = "Multi-State Machine")
	void HandleInput(const FGameplayTag InputActionTag, const FInputActionValue& Value, const FGameplayTag RelativeStateMachine);

	UFUNCTION(BlueprintCallable, Category = "Multi-State Machine")
	void PauseAll(const bool bResetToEntryState = false);

	UFUNCTION(BlueprintCallable, Category = "Multi-State Machine")
	void Pause(FGameplayTag StateMachineTag, const bool bResetToEntryState = false);
	
	UFUNCTION(BlueprintCallable, Category = "Multi-State Machine")
	void UnPauseAll();

	UFUNCTION(BlueprintCallable, Category = "Multi-State Machine")
	void UnPause(FGameplayTag StateMachineTag);

	UFUNCTION(BlueprintCallable, Category = "Multi-State Machine")
	void AddStateMachine(UStateMachineConfig* StateMachineData);

	UFUNCTION(BlueprintCallable, Category = "Multi-State Machine")
	FReplicatedData GetReplicatedData(FGameplayTag StateMachineTag) const;

private:

	void CheckStateToInterrupt(const UStateBase* Interrupter);

	UFUNCTION(BlueprintCallable, Category = "Multi-State Machine", meta = (AllowPrivateAccess = "true"))
	void Initialize();
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void StateChanged(FGameplayTag StateMachineTag, FGameplayTag PreviousState, FGameplayTag NewState);

	UFUNCTION()
	void ReplicatedDataChanged(const FReplicatedData& NewData);
};
