// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "HDGE_Buff.generated.h"

struct FGameplayEffectCustomExecutionParameters;
struct FGameplayEffectCustomExecutionOutput;
struct FBattleStateParams;

USTRUCT(BlueprintType)
struct FBuffEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
public:
	bool IsRemainTurn()
	{
		return RemainTurn <= 0;
	}
public:
	int32 BuffID;

	UPROPERTY()
	FBattleStateParams BattleStateParams;

	int32 RemainTurn;
	bool IsStackable;
};

UCLASS()
class PROJECTH_API UHDBuffExecutionCalculation :public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UHDBuffExecutionCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};

UCLASS()
class PROJECTH_API UHDGE_Buff : public  UGameplayEffect
{
	GENERATED_BODY()
public:
	UHDGE_Buff();
};
