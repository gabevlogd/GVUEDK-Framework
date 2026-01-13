

#include "SpawnSystem/Factories/SpawnMethodFactory.h"
#include "SpawnSystem/SpawnMethods/Base/SpawnMethod.h"




USpawnMethod* USpawnMethodFactory::CreateSpawnMethod(USpawnMethod* Template, USpawnManager* InSpawnManager)
{
	if (!IsValid(InSpawnManager))
	{
		UE_LOG(LogSpawnMethodFactory, Error, TEXT("USpawnMethodFactory::CreateSpawnMethod called with invalid SpawnManager, return nullptr"));
		return nullptr;
	}

	USpawnMethod* NewSpawnMethod = NewObject<USpawnMethod>(InSpawnManager, Template->GetClass());
	if (NewSpawnMethod->Init(Template, InSpawnManager))
	{
		return NewSpawnMethod;
	}

	UE_LOG(LogSpawnMethodFactory, Error, TEXT("USpawnMethodFactory::CreateSpawnMethod failed to initialize new SpawnMethod, return nullptr"));
	return nullptr;
}