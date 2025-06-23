// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDGameAbilityParam.generated.h"

USTRUCT()
struct FGameAbilityParam
{
	GENERATED_BODY()
public:
	virtual FGameAbilityParam* Clone() const
	{
		return new FGameAbilityParam(*this);
	}
};