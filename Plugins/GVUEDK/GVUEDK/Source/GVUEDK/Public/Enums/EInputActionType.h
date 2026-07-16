// Copyright Villains, Inc. All Rights Reserved.

#pragma once

UENUM(BlueprintType)
enum class EInputActionType : uint8
{
	None UMETA(DisplayName = "None"),
	Walk UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run"),
	Jump UMETA(DisplayName = "Jump"),
	Dodge UMETA(DisplayName = "Dodge"),
	Shoot UMETA(DisplayName = "Shoot"),
	StopShoot UMETA(DisplayName = "StopShoot"),
	Aim UMETA(DisplayName = "Aim"),
	StopAim UMETA(DisplayName = "StopAim"),
	Hook UMETA(DisplayName = "Hook"),
	Interact UMETA(DisplayName = "Interact"),
	Reload UMETA(DisplayName = "Reload"),
	MeleeAttack UMETA(DisplayName = "MeleeAttack")
};
