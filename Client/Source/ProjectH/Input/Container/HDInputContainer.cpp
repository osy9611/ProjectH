// Fill out your copyright notice in the Description page of Project Settings.


#include "HDInputContainer.h"

FString HDInputContainer::GetMappingContextPath(EInputMappingType Type)
{
	switch (Type)
	{
	case EInputMappingType::Battle:
		return "/Game/Input/Battle/IMC_Battle.IMC_Battle";
	default:
		return FString();
	}
}

FString HDInputContainer::GetHDInputConfigPath(EInputMappingType Type)
{
	switch (Type)
	{
	case EInputMappingType::Battle:
		return "/Game/Input/Battle/InputData_Battle.InputData_Battle";
	default:
		return FString();
	}
}
