// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Behaviours/MovementBehaviours/Base/GHMovementBehaviourBase.h"
#include "Behaviours/SearchBehaviours/Base/GHSearchBehaviourBase.h"
#include "Interfaces/GrabPoint.h"
#include "GrapplingHookComponent.generated.h"

class UGHSearchBehaviourBase;
class UGHMovementBehaviourBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartHooking);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHookMotionStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPerformHookMotion);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopHooking);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInterruptHooking);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHookAttached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FModeChanged, FGameplayTag, ModeTag);

USTRUCT(BlueprintType, Blueprintable)
struct FGrapplingHookMode
{
	GENERATED_BODY()

	FGrapplingHookMode()
	{
		MovementBehaviour = nullptr;
		SearchBehaviour = nullptr;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ModeTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	UGHMovementBehaviourBase* MovementBehaviour;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	UGHSearchBehaviourBase* SearchBehaviour;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRAPPLINGHOOKSYSTEM_API UGrapplingHookComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FStartHooking OnStartHooking;
	
	UPROPERTY(BlueprintAssignable)
	FHookMotionStarted OnHookMotionStarted;
	
	UPROPERTY(BlueprintAssignable)
	FPerformHookMotion OnPerformHookMotion;
	
	UPROPERTY(BlueprintAssignable)
	FStopHooking OnStopHooking;
	
	UPROPERTY(BlueprintAssignable)
	FInterruptHooking OnInterruptHooking;

	UPROPERTY(BlueprintAssignable)
	FHookAttached OnHookAttached;

	UPROPERTY(BlueprintAssignable)
	FModeChanged OnModeChanged;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FGrapplingHookMode> Modes;

private:
	UPROPERTY()
	TMap<FGameplayTag, FGrapplingHookMode> ModeMap;
	
	UPROPERTY()
	FGrapplingHookMode CurrentMode;
	
	UPROPERTY()
	ACharacter* OwnerCharacter;
	
	IGrabPoint* TargetGrabPoint;
	bool bTargetAcquired;

	FVector HookLocation;
	bool bHookHitObstacle;
	
	bool bInitialized;

	bool bOwnerIsMovingTowardTarget;

public:
	UGrapplingHookComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool StartHooking() { return CurrentMode.MovementBehaviour->StartHooking(); }
	UFUNCTION(BlueprintCallable)
	void StopHooking() const { CurrentMode.MovementBehaviour->StopHooking(); }
	
	UFUNCTION(BlueprintCallable)
	float GetElapsedTime() const { return CurrentMode.MovementBehaviour->GetElapsedTime(); }
	UFUNCTION(BlueprintCallable)
	float GetStartDelay() const { return CurrentMode.MovementBehaviour->GetStartDelay(); }
	
	UFUNCTION(BlueprintCallable)
	FVector GetTargetGrabPointLocation() const { return bTargetAcquired ? TargetGrabPoint->Execute_GetLocation(TargetGrabPoint->_getUObject()) : FVector::ZeroVector; }
	UFUNCTION(BlueprintCallable)
	FVector GetHookLocation() const { return HookLocation; }
	
	UFUNCTION(BlueprintCallable)
	void ChangeMode(const FGameplayTag ModeTag);
	
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetCurrentModeTag() const { return CurrentMode.ModeTag; }

	UFUNCTION(BlueprintCallable)
	FGrapplingHookMode GetMode(const FGameplayTag ModeTag) const { return ModeMap.Contains(ModeTag) ? ModeMap[ModeTag] : FGrapplingHookMode(); }
	
	UFUNCTION(BlueprintCallable)
	UGHMovementBehaviourBase* GetCurrentMovementBehaviour() const { return CurrentMode.MovementBehaviour; }
	UFUNCTION(BlueprintCallable)
	UGHSearchBehaviourBase* GetCurrentSearchBehaviour() const { return CurrentMode.SearchBehaviour; }

	ACharacter* GetOwnerCharacter() const { return OwnerCharacter; }
	IGrabPoint* GetTargetGrabPoint() const { return TargetGrabPoint; }
	
	void SetTargetGrabPoint(IGrabPoint* GrabPoint) { TargetGrabPoint = GrabPoint; bTargetAcquired = TargetGrabPoint != nullptr; }
	void SetHookLocation(const FVector& Location) { HookLocation = Location; }
	void SetHookHitObstacle(const bool Value) { bHookHitObstacle = Value; }

	UFUNCTION(BlueprintCallable)
	bool IsTargetAcquired() const { return bTargetAcquired; }
	bool HasHookHitObstacle() const { return bHookHitObstacle; }

	UFUNCTION(BlueprintCallable)
	bool IsOwnerMovingTowardTarget() const { return bOwnerIsMovingTowardTarget; }
	UFUNCTION(BlueprintCallable)
	void SetOwnerMovingTowardTarget(const bool Value) { bOwnerIsMovingTowardTarget = Value; }

private:
	// Returns true if the modes are initialized successfully
	bool InitializeModes();
	
	
};


