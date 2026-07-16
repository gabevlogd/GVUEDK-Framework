#include "Data/PopUpData.h"
#include "Interfaces/PopUp.h"

bool FPopUpData::IsValid() const
{
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("PopUpData: WidgetClass is not valid for PopUpTag %s."), *Tag.ToString());
		return false;
	}
    
	if (!WidgetClass->ImplementsInterface(UPopUp::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("PopUpData: WidgetClass %s does not implement IPopUp interface for PopUpTag %s."), *WidgetClass->GetName(), *Tag.ToString());
		return false;
	}

	return true;
}