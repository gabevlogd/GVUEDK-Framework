// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Behaviours/Base/GrapplingHookBehaviourBase.h"
#include "GHSearchBehaviourBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew)
class GRAPPLINGHOOKSYSTEM_API UGHSearchBehaviourBase : public UGrapplingHookBehaviourBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bShowDebug;
};
