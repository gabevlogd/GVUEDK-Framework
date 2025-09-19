// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DodgerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartDodge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDodge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopDodge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDodgeCooldownComplete);

#define GRAVITY 980.665f // cm/s^2

UENUM(BlueprintType) 
enum class EDefaultDirection : uint8
{
	CharacterForward UMETA(DisplayName = "Character Forward"),
	CharacterBackward UMETA(DisplayName = "Character Backward"),
	CharacterLeft UMETA(DisplayName = "Character Left"),
	CharacterRight UMETA(DisplayName = "Character Right"),
	SightForward UMETA(DisplayName = "Sight Forward"),
	SightBackward UMETA(DisplayName = "Sight Backward"),
	SightLeft UMETA(DisplayName = "Sight Left"),
	SightRight UMETA(DisplayName = "Sight Right")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GVUEDK_API UDodgerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FStartDodge OnStartDodge;
	UPROPERTY(BlueprintAssignable)
	FDodge OnDodge;
	UPROPERTY(BlueprintAssignable)
	FStopDodge OnStopDodge;
	UPROPERTY(BlueprintAssignable)
	FDodgeCooldownComplete OnDodgeCooldownComplete;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", EditCondition = "!bApplyUniformVelocity"))
	UCurveFloat* SpeedCurve;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", EditCondition = "bApplyUniformVelocity", ClampMin = "0.0"))
	float LinearSpeed = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	EDefaultDirection DefaultDirection = EDefaultDirection::CharacterForward;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", EditCondition = "bOrientRotationToMovement", ClampMin = "0.0", ToolTip = "If 0, rotation will be instant"))
	float RotationSpeed = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", EditCondition = "bApplyGravity", ClampMin = "0.0"))
	float GravityScale = 1.f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float DodgeDuration = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float DodgeCooldown = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float StartDelay = 0.f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bApplyGravity = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bApplyUniformVelocity = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ToolTip = "If true, the character will dodge in the direction specified by the WorldDirection parameter of StartDodge (if the parameter is zero, the character will dodge in the direction of the DefaultDirection)"))
	bool bDirectionalDodge = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bOrientRotationToMovement = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bConserveMomentum = false;

	UPROPERTY()
	ACharacter* Owner;
	
	FVector TargetVelocity;
	FVector DefaultLocalDirection;
	const FTransform* RelativeDirectionTransform;
	
	FRotator TargetRotation;
	FRotator CurrentRotation;
	
	float ElapsedTime = 0.f;
	bool bIsDodging = false;
	bool bIsCoolingDown = false;
	bool bInitialized = false;
	
public:
	UDodgerComponent();
	UFUNCTION(BlueprintCallable)
	void StartDodge(FVector WorldDirection = FVector::ZeroVector);
	UFUNCTION(BlueprintCallable)
	void StopDodge();
	UFUNCTION(BlueprintCallable)
	bool IsCoolingDown() const { return bIsCoolingDown; }
	UFUNCTION(BlueprintCallable)
	float GetCooldownDuration() const { return DodgeCooldown; }
	
private:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Initialize();
	void PerformDodge(float DeltaTime);
	void InitDefaultDirection();
	void StartCooldown();
	float GetSpeed() const;
	FVector GetDefaultDirection() const;

};
