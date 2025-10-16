// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "HDGEComp_Damage_Base.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UHDGEComp_Damage_Base : public UGameplayEffectComponent
{
	GENERATED_BODY()
public:
	virtual void OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;
};
