

#include "SpawnSystem/SpawnMethods/Base/SpawnMethod.h"


bool USpawnMethod::Init(USpawnMethod* Template, USpawnManager* InSpawnManager)
{

#if WITH_EDITOR
	bShowDebug = Template->bShowDebug;
#endif
	
	return true;
}