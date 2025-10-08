// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGEComp_DeBuff.h"
#include "ProjectH/LogChannels.h"

void UHDGEComp_DeBuff::OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	Super::OnGameplayEffectExecuted(ActiveGEContainer, GESpec, PredictionKey);
	UE_LOG(HDLog, Log, TEXT("[UHDGEComp_DeBuff] OnGameplayEffectExecuted"));
}
