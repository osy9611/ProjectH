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
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Battle.Buff"));
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
		const_cast<FBuffEffectContext*>(BuffContext)->BuffExecuteType = BuffData->ExectueType;
	}

	OutExecutionOutput.AddOutputModifier(CreateData(BuffContext, BuffData));
}

FGameplayModifierEvaluatedData UHDBuffExecutionCalculation::CreateData(const FBuffEffectContext* BuffContext, FBuffData* BuffData) const
{
	if(!BuffData || !BuffContext)
		return FGameplayModifierEvaluatedData();

	FGameplayAttribute Attribute;
	float Amount;
	switch (BuffData->BuffType)
	{
	case EBuffType::AddPATK:
	case EBuffType::LowPATK:
		Attribute = UHDAttributeSet::GetG1_PATKAttribute();
		break;
	case EBuffType::AddEATK:
	case EBuffType::LowEATK:
		Attribute = UHDAttributeSet::GetG1_EATKAttribute();
		break;
	case EBuffType::AddPDEF:
	case EBuffType::LowPDEF:
		Attribute = UHDAttributeSet::GetG1_PDEFAttribute();
		break;
	case EBuffType::AddEDEF:
	case EBuffType::LowEDEF:
		Attribute = UHDAttributeSet::GetG1_EDEFAttribute();
		break;
	}

	Amount = BuffData->Value * const_cast<FBuffEffectContext*>(BuffContext)->StackCount;

	//Low 타입의 경우에는 감소이기 때문에 -1로 계산함
	if (BuffData->BuffType == EBuffType::LowPATK || BuffData->BuffType == EBuffType::LowEATK 
		|| BuffData->BuffType == EBuffType::LowPDEF || BuffData->BuffType == EBuffType::LowEDEF)
		Amount *= -1;

	return FGameplayModifierEvaluatedData(Attribute, EGameplayModOp::Additive,Amount);
}


