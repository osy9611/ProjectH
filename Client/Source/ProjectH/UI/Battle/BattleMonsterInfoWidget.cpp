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
UBattleMonsterInfoWidget::UBattleMonsterInfoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UBattleMonsterInfoWidget::OnInit(const FMonsterData MonsterData)
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
}

void UBattleMonsterInfoWidget::UpdateHP(AActor* Actor)
{
	UHDAbilitySystemComponent* ASC = UtilFunc::GetASC(Actor);
	if (!ASC)
		return;

	const UHDAttributeSet_Monster* AttributeSet = ASC->GetSet<UHDAttributeSet_Monster>();

	float OriginHP = AttributeSet->GetOriginHP();
	float CurrentHP = AttributeSet->GetHP();

	float Amount = CurrentHP / OriginHP;
	HPBar->SetPercent(Amount);
}
