// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGEComp_Buff.h"
#include "AbilitySystemComponent.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "ProjectH/AbilitySystem/GameEffect/HDGE_Buff.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/AbilitySystem/HDAbilitySystemComponent.h"

void UHDGEComp_Buff::OnGameplayEffectChanged()
{
	Super::OnGameplayEffectChanged();

	UE_LOG(HDLog, Log, TEXT("[UHDGEComp_Buff] OnGameplayEffectChanged"));
}

void UHDGEComp_Buff::OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	Super::OnGameplayEffectExecuted(ActiveGEContainer, GESpec, PredictionKey);
	UE_LOG(HDLog, Log, TEXT("[UHDGEComp_Buff] OnGameplayEffectExecuted"));
}

void UHDGEComp_Buff::OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	Super::OnGameplayEffectApplied(ActiveGEContainer, GESpec, PredictionKey);
	UE_LOG(HDLog, Log, TEXT("[UHDGEComp_Buff] OnGameplayEffectApplied"));

	const FGameplayEffectContextHandle& ContextHandle = GESpec.GetEffectContext();
	if (!ContextHandle.IsValid())
		return;

	FBuffEffectContext* BuffContext = (FBuffEffectContext*)ContextHandle.Get();
	if (!BuffContext)
		return;

	FBuffData BuffData = BuffContext->BuffData;

	//Set Stack Limit Count
	const UGameplayEffect* GE = GESpec.Def;
	if (!GE)
		return;
	UGameplayEffect* MutableGE = const_cast<UGameplayEffect*>(GE);
	MutableGE->StackLimitCount = BuffData.MaxStack;

	//Stack °è»ê
	float StackCount = GESpec.StackCount ==1 ? 0 : GESpec.StackCount;
	float Amount = BuffData.Value + (BuffData.Value * StackCount * 0.5f);

	if (BuffData.BuffType == EBuffType::LowPATK || BuffData.BuffType == EBuffType::LowEATK
		|| BuffData.BuffType == EBuffType::LowPDEF || BuffData.BuffType == EBuffType::LowEDEF)
		Amount *= -1;

	UAbilitySystemComponent* TargetASC = ActiveGEContainer.Owner;

	if (!TargetASC)
		return;

	const UHDAttributeSet* SourceAttributeSet = TargetASC->GetSet<UHDAttributeSet>();

	UE_LOG(HDLog, Log, TEXT("Before Data : %f"), SourceAttributeSet->GetG1_PATK());

	ActiveGEContainer.ApplyModToAttribute(
		GetBuffAttribute(BuffData.BuffType), EGameplayModOp::Override, Amount);

	UE_LOG(HDLog, Log, TEXT("After Data : %f"), SourceAttributeSet->GetG1_PATK());
	UE_LOG(HDLog, Log, TEXT("After Stack : %d"), GESpec.StackCount);
}

FGameplayAttribute UHDGEComp_Buff::GetBuffAttribute(EBuffType Type) const
{
	switch (Type)
	{
	case EBuffType::AddPATK:
	case EBuffType::LowPATK:
		return UHDAttributeSet::GetG1_PATKAttribute();
	case EBuffType::AddEATK:
	case EBuffType::LowEATK:
		return UHDAttributeSet::GetG1_EATKAttribute();
	case EBuffType::AddPDEF:
	case EBuffType::LowPDEF:
		return UHDAttributeSet::GetG1_PDEFAttribute();
	case EBuffType::AddEDEF:
	case EBuffType::LowEDEF:
		return UHDAttributeSet::GetG1_EDEFAttribute();
	}
	return FGameplayAttribute();
}
