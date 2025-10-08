// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "HDGE_DeBuff.generated.h"

struct FBattleStateParams;

USTRUCT(BlueprintType)
struct FDeBuffEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
public:
	int32 DeBuffID;

	UPROPERTY()
	FDeBuffData Data;

	UPROPERTY()
	FBattleStateParams BattleStateParams;

	EBuffExecuteType BuffExecuteType;
};
UCLASS()
class PROJECTH_API UHDGE_DeBuff : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UHDGE_DeBuff();
};
