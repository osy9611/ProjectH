// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGE_Buff.h"
#include "AbilitySystemGlobals.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/AbilitySystem//AttributeSet/HDAttributeSet.h"

UHDGE_Buff::UHDGE_Buff()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	Executions.Add(FGameplayEffectExecutionDefinition(UHDBuffExecutionCalculation::StaticClass()));
	InheritableOwnedTagsContainer
}

UHDBuffExecutionCalculation::UHDBuffExecutionCalculation()
{
}

void UHDBuffExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

	const FBuffEffectContext* BuffContext = static_cast<FBuffEffectContext*>(ContextHandle.Get());
	if (!BuffContext)
		return;

	FBuffData* BuffData = UtilFunc_Data::GetTableData<FBuffData>(GetWorld(), FString::FromInt(BuffContext->BuffID));
	if (!BuffData)
		return;

	//´ë»óÀÚ
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetASC)
		return;

	const UHDAttributeSet* TargetAttr = TargetASC->GetSet<UHDAttributeSet>();
	if (!TargetAttr)
		return;

	//RemainTurn = BuffData->TurnCount;
}
