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
	//InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Buff"));
	Executions.Add(FGameplayEffectExecutionDefinition(UHDBuffExecutionCalculation::StaticClass()));
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

	//대상자
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetASC)
		return;

	const UHDAttributeSet* TargetAttr = TargetASC->GetSet<UHDAttributeSet>();
	if (!TargetAttr)
		return;

	if (BuffContext->RemainTurn == 0)
	{
		const_cast<FBuffEffectContext*>(BuffContext)->RemainTurn = BuffData->TurnCount;
		const_cast<FBuffEffectContext*>(BuffContext)->IsStackable = BuffData->Stackable;
	}

	//버프 적용
	if (BuffData->ExectueType == EBuffExecuteType::Instant)
	{
		if (BuffData->BuffType == EBuffType::Dot)
		{
			//TODO :  어떻게 처리 해야할까 고민을 좀 해야할듯
		}
		else
		{

		}
	}
}
