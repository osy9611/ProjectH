// Fill out your copyright notice in the Description page of Project Settings.


#include "HDUIManagerSubsystem.h"
UHDUIManagerSubsystem::UHDUIManagerSubsystem()
{
}

void UHDUIManagerSubsystem::RegisterInputData()
{
	//PlayerController* PlayerController = 
}

UCommonActivatableWidget* UHDUIManagerSubsystem::FindWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	return Super::FindWidget(WidgetClass);
}
