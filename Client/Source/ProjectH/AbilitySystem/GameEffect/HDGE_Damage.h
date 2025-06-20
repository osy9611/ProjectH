// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "HDGE_Damage.generated.h"

struct FGameplayEffectCustomExecutionParameters;
struct FGameplayEffectCustomExecutionOutput;
struct FBattleStateParams;
USTRUCT(BlueprintType)
struct FDamageEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FGameplayTag SkillTag;

	UPROPERTY()
	FBattleStateParams BattleStateParams;
};

UCLASS()
class PROJECTH_API UHDDamageExecutionCalculation :public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UHDDamageExecutionCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};

UCLASS()
class PROJECTH_API UHDGE_Damage : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UHDGE_Damage();
};
