// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGE_Buff.h"
#include "AbilitySystemGlobals.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/AbilitySystem/GameEffectComp/HDGEComp_Buff.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"

UHDGE_Buff::UHDGE_Buff()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	const UEnum* EnumPtr = StaticEnum<EBuffType>();
	TArray<FText> BuffNames = UtilFunc::EnumToText(EnumPtr, static_cast<int8>(EBuffType::Max));

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;

	UHDGEComp_Buff* GEComp = CreateDefaultSubobject<UHDGEComp_Buff>(TEXT("HDGEComp_Buff"));
	GEComponents.Add(GEComp);
}
