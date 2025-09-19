// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Components/ActorComponent.h"
#include "InputBindingComponent.generated.h"

class UEnhancedPlayerMappableKeyProfile;
class UEnhancedInputUserSettings;
DEFINE_LOG_CATEGORY_STATIC(LogInputBinding, All, All);

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INPUTBINDINGSYSTEM_API UInputBindingComponent : public UActorComponent
{
	GENERATED_BODY()


private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> InputMappingContexts;
	
	UPROPERTY(BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UEnhancedInputUserSettings* InputUserSettings;

	UPlayerMappableKeySettings* PlayerMappableKeySettings;
	
public:
	
	UInputBindingComponent();

protected:

	UFUNCTION(BlueprintImplementableEvent)
	bool IsMappingContextRegistered(UInputMappingContext* IMC) const;
	UFUNCTION(BlueprintImplementableEvent)
	bool RegisterInputMappingContext(UInputMappingContext* IMC) const;
	UFUNCTION(BlueprintImplementableEvent)
	UEnhancedPlayerMappableKeyProfile* GetCurrentKeyProfile() const;

private:

	void Initialize();

	virtual void BeginPlay() override;
};
