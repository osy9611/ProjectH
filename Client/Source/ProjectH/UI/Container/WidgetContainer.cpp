// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetContainer.h"

FString WidgetContainer::GetUIPath(EUIType type)
{
	switch (type)
	{
	case EUIType::Battle:		
		return "/Game/Widgets/Battle/WBP_Battle.WBP_Battle_C";
	default:
		return FString();
	}
}
