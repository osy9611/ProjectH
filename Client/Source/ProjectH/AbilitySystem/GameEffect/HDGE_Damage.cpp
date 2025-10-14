// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGE_Damage.h"
#include "AbilitySystemGlobals.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/AbilitySystem/GameEffectComp/HDGEComp_Damage_Base.h"
UHDGE_Damage::UHDGE_Damage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant; //한 프레임에 바로 실행되는 GE 타입
	UHDGEComp_Damage_Base* GEComp = CreateDefaultSubobject<UHDGEComp_Damage_Base>(TEXT("HDGEComp_Damage_Base"));
	GEComponents.Add(GEComp);
}
