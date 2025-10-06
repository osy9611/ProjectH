// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "HDGEComp_Buff.generated.h"

struct FGameplayAttribute;
UCLASS()
class PROJECTH_API UHDGEComp_Buff : public UGameplayEffectComponent
{
	GENERATED_BODY()
public:
	virtual void OnGameplayEffectChanged() override;
	virtual void OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;
	virtual void OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;

	FGameplayAttribute GetBuffAttribute(EBuffType Type) const;
};
