// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GvCharacter.generated.h"

class UGvCharacterMovementComponent;

UCLASS()
class CUSTOMMOVEMENTSYSTEM_API AGvCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGvCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual bool CanJumpInternal_Implementation() const override;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UGvCharacterMovementComponent* CustomCharacterMovement;
};
