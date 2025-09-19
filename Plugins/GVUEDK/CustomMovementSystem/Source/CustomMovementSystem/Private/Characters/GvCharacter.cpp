// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GvCharacter.h"
#include "Components/GvCharacterMovementComponent.h"


AGvCharacter::AGvCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UGvCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGvCharacter::BeginPlay()
{
	CustomCharacterMovement = Cast<UGvCharacterMovementComponent>(GetCharacterMovement());
	Super::BeginPlay();
}

bool AGvCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() || CustomCharacterMovement->CanCoyoteJump();
}
