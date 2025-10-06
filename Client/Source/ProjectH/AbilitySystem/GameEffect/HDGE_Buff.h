// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "HDGE_Buff.generated.h"

struct FGameplayEffectCustomExecutionParameters;
struct FGameplayEffectCustomExecutionOutput;
struct FBattleStateParams;
struct FGameplayModifierEvaluatedData;
struct FBuffData;

USTRUCT(BlueprintType)
struct FBuffEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
public:
	int32 BuffID;

	UPROPERTY()
	FBuffData BuffData;

	UPROPERTY()
	FBattleStateParams BattleStateParams;

	EBuffExecuteType BuffExecuteType;
};

UCLASS()
class PROJECTH_API UHDGE_Buff : public  UGameplayEffect
{
	GENERATED_BODY()
public:
	UHDGE_Buff();
};
