// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleMonsterInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/AbilitySystem/HDAbilitySystemComponent.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet_Monster.h"
#include "ProjectH/UI/Data/HDMVVM_MonsterInfo.h"
UBattleMonsterInfoWidget::UBattleMonsterInfoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UBattleMonsterInfoWidget::OnInit(UHDAbilitySystemComponent* ASC, const FMonsterData& MonsterData)
{
	FString MonsterName = UtilFunc_Data::GetCommonString(GetWorld(), MonsterData.MonsterName);
	NameText->SetText(FText::FromString(MonsterName));

	FMonsterStatusData* StatusData = UtilFunc_Data::GetTableData<FMonsterStatusData>(GetWorld(), FString::FromInt(MonsterData.StatusNo));
	if (!StatusData)
	{
		UE_LOG(HDLog, Log, TEXT("[BattleMonsterInfoWidget] StatusData is nullptr TableNo %d"), MonsterData.StatusNo);
		return;
	}

	ToughnessText->SetText(FText::AsNumber(StatusData->Toughness));

	if (!ASC)
	{
		UE_LOG(HDLog, Log, TEXT("[BattleMonsterInfoWidget] HDAbilitySystemComponent is nullptr"));
		return;
	}


	MonsterInfo = NewObject<UHDMVVM_MonsterInfo>(this);
	MonsterInfo->Init(ASC);
	MonsterInfo->BindField(UHDMVVM_MonsterInfo::FFieldNotificationClassDescriptor::CurrentHP, this, &ThisClass::HandleUpdateHPBar);
}

void UBattleMonsterInfoWidget::HandleUpdateHPBar(UObject* Object, UE::FieldNotification::FFieldId FieldId)
{
	if (!HPBar)
		return;

	const UHDMVVM_MonsterInfo* VM = Cast<UHDMVVM_MonsterInfo>(Object);
	if (!VM) return;

	HPBar->SetPercent(VM->CurrentHP);
}
