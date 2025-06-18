// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HDMessageExtensions.generated.h"

USTRUCT(BlueprintType)
struct FBattleEventSelectActivate
{
	GENERATED_BODY()

public:
	FGameplayTag SelectTag;
};