// Fill out your copyright notice in the Description page of Project Settings.


#include "HDMVVM_MonsterInfo.h"
#include "AbilitySystemComponent.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/AbilitySystem/HDAbilitySystemComponent.h"
#include "FieldNotificationId.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"

void UHDMVVM_MonsterInfo::Init(UAbilitySystemComponent* InASC)
{
	Super::Init(InASC);

	if (!InASC)
		return;
		
	BindASCValueChange(UHDAttributeSet::GetHPAttribute(), this, &ThisClass::HandleDataChanged);
	
	const UHDAttributeSet* AttributeSet = InASC->GetSet<UHDAttributeSet>();

	if (UE_MVVM_SET_PROPERTY_VALUE(CurrentHP, AttributeSet->GetHP()))
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CurrentHP);
	if (UE_MVVM_SET_PROPERTY_VALUE(OriginHP, AttributeSet->GetOriginHP()))
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(OriginHP);
}

void UHDMVVM_MonsterInfo::HandleDataChanged(const FOnAttributeChangeData& Data)
{
	if (Data.Attribute == UHDAttributeSet::GetHPAttribute())
	{
		CurrentHP = Data.NewValue / OriginHP;
		/*if (UE_MVVM_SET_PROPERTY_VALUE(CurrentHP, Data.NewValue / OriginHP))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CurrentHP);
		}*/
		//UE_MVVM_SET_PROPERTY_VALUE(CurrentHP, Data.NewValue / OriginHP);

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CurrentHP);
	}
}
