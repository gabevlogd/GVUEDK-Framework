// Copyright Villains, Inc. All Rights Reserved.


#include "Utility/FGvDebug.h"


FGvDebug::FGvDebug()
{
}

FGvDebug::~FGvDebug()
{
}

void FGvDebug::Log(const FString& LogMsg, const bool bPrintOnScreen, const float DisplayTime)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *LogMsg);
	if (bPrintOnScreen && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, DisplayTime, FColor::Green, LogMsg);
	}
}

void FGvDebug::Warning(const FString& WarningMsg, const bool bPrintOnScreen, const float DisplayTime)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *WarningMsg);
	if (bPrintOnScreen && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, DisplayTime, FColor::Yellow, WarningMsg);
	}
}

void FGvDebug::Error(const FString& ErrorMsg, const bool bPrintOnScreen, const float DisplayTime)
{
	UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
	if (bPrintOnScreen && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, DisplayTime, FColor::Red, ErrorMsg);
	}
	
}
