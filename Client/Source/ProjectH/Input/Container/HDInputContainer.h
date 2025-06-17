// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDInputContainer.generated.h"


UENUM(BlueprintType)
enum class EInputMappingType :uint8
{
	Default,
	Battle,
};

class PROJECTH_API HDInputContainer
{
public:
	static FString GetMappingContextPath(EInputMappingType Type);
	static FString GetHDInputConfigPath(EInputMappingType Type);
};
