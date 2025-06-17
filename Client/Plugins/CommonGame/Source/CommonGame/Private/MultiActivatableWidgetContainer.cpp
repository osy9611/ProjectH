// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiActivatableWidgetContainer.h"

void UMultiActivatableWidgetContainer::AddAndActivateWidgetInstance(UCommonActivatableWidget* Widget)
{
	if (!Widget)
		return;

	if (!GetWidgetList().Contains(Widget))
	{
		//AddChild(Widget);
	}

}

void UMultiActivatableWidgetContainer::DeactivateAllWidgets()
{
}
