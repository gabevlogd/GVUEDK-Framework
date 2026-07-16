// Copyright (c) 2025, Gabevlogd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "State/StateBase.h"
#include "Components/ActorComponent.h"
#include "Data/StateMachineConfig.h"
#include "StateMachineComponent.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogStateMachine, All, All);

USTRUCT(BlueprintType, Blueprintable)
struct FReplicatedData
{
	GENERATED_BODY()

	FReplicatedData()
		: CurrentStateTag(FGameplayTag::EmptyTag)
		, PreviousStateTag(FGameplayTag::EmptyTag)
		, StateMachineTag(FGameplayTag::EmptyTag)
	{
	}

	FReplicatedData(const FGameplayTag InCurrentStateTag, const FGameplayTag InPreviousStateTag, const FGameplayTag InStateMachineTag)
		: CurrentStateTag(InCurrentStateTag)
		, PreviousStateTag(InPreviousStateTag)
		, StateMachineTag(InStateMachineTag)
	{
	}

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CurrentStateTag;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag PreviousStateTag;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag StateMachineTag;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStateChanged, FGameplayTag, StateMachineTag, FGameplayTag, PreviousState, FGameplayTag, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReplicatedDataChanged, const FReplicatedData&, ReplicatedData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), EditInlineNew)
class STATEMACHINESYSTEM_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category="State Machine")
	FOnStateChanged OnStateChanged;

	UPROPERTY(BlueprintAssignable, Category="State Machine")
	FOnReplicatedDataChanged OnReplicatedDataChanged;
	
private:

	friend class UMultiStateMachineComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Machine", meta = (AllowPrivateAccess = "true"))
	FGameplayTag StateMachineTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "State Machine", DisplayName = "Entry State", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UStateBase> EntryStateClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "State Machine", DisplayName = "States", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UStateBase>> StateClasses;

	UPROPERTY(BlueprintReadOnly, Category = "State Machine", meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, UStateBase*> StatesMap;

	UPROPERTY(BlueprintReadOnly, Category = "State Machine", meta = (AllowPrivateAccess = "true"))
	UStateBase* EntryState;

	UPROPERTY(BlueprintReadOnly, Category = "State Machine", meta = (AllowPrivateAccess = "true"))
	UStateBase* CurrentState;

	UPROPERTY(BlueprintReadOnly, Category = "State Machine", meta = (AllowPrivateAccess = "true"))
	UStateBase* PreviousState;

	UPROPERTY(BlueprintReadOnly, Category = "State Machine", meta = (AllowPrivateAccess = "true"), ReplicatedUsing=OnRep_ReplicatedData)
	FReplicatedData ReplicatedData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Machine", meta = (AllowPrivateAccess = "true"))
	bool bReplicateData;

	UPROPERTY()
	AActor* Context;

	UPROPERTY()
	UMultiStateMachineComponent* MultiStateMachine;
	
	bool bIsRunning;
	bool bInitialized;
	bool bPaused;

public:
	
	UStateMachineComponent();

	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "State Machine")
	FGameplayTag GetStateMachineTag() const { return StateMachineTag; }
	
	UFUNCTION(BlueprintCallable, Category = "State Machine")
	UStateBase* ChangeState(const FGameplayTag NextState);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "State Machine")
	bool TryGetState(const FGameplayTag StateTag, UStateBase*& OutState) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "State Machine")
	UStateBase* GetState(const FGameplayTag StateTag) const;
	
	UFUNCTION(Blueprintable, BlueprintPure, Category = "State Machine")
	UStateBase* GetCurrentState() const { return CurrentState; }
	
	UFUNCTION(Blueprintable, BlueprintPure, Category = "State Machine")
	UStateBase* GetPreviousState() const { return PreviousState; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "State Machine")
	FGameplayTag GetCurrentStateTag() const { return IsValid(CurrentState) ? CurrentState->StateTag : FGameplayTag::EmptyTag; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "State Machine")
	FGameplayTag GetPreviousStateTag() const { return IsValid(PreviousState) ? PreviousState->StateTag : FGameplayTag::EmptyTag; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "State Machine")
	bool GetIsRunning() const { return bIsRunning; }

	/**
     *  Returns the new state entered by this state machine after handling the input if any, otherwise returns nullptr
     */
	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void HandleInput(const FGameplayTag InputActionTag, const FInputActionValue& Value);
	
	void InterruptCurrentState(const FGameplayTag Interrupter);

	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void Pause(const bool bResetToEntryState = false);

	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void UnPause() { bPaused = false; }

	UFUNCTION(BlueprintCallable, Category = "State Machine")
	FReplicatedData GetReplicatedData() const { return ReplicatedData; }
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool WantsReplication() const { return bReplicateData; }
	
private:

	void SetConfigData(const UStateMachineConfig* StateMachineData);
	
	void Initialize();

	void ChangeState(UStateBase* NextState);

	bool IsNegated(const UStateBase* StateToCheck) const;

	UFUNCTION()
	void OnRep_ReplicatedData();

	UFUNCTION(Server, Reliable)
	void Server_ReplicateData(const FReplicatedData& InReplicatedData);

};
