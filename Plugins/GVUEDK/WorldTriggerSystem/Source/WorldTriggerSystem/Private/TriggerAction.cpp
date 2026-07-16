#include "TriggerAction.h"
#include "WorldTriggerContainer.h"

AWorldTriggerContainer* UTriggerAction::GetContainer() const
{
	return GetTypedOuter<AWorldTriggerContainer>();
}
