// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet_Player.h"
#include "ProjectH/Data/GenerateTableData.h"
UBattleCharInfoWidget::UBattleCharInfoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UBattleCharInfoWidget::OnInit(UHDAttributeSet* AttributeSet)
{
	if (!AttributeSet)
		return;

	//데이터 입력 전에 한번 스테이터스 업데이트
	AttributeSet->OnUpdateStatus();

	FCharacterData* CharData = AttributeSet->GetTableData<FCharacterData>();
	if (!CharData)
		return;

	UHDAttributeSet_Player* PlayerAttribute = Cast<UHDAttributeSet_Player>(AttributeSet);
	if (!PlayerAttribute)
		return;

	Attibute = PlayerAttribute;

	FString CharName = UtilFunc_Data::GetCommonString(GetWorld(), CharData->CharName);
	CharNameText->SetText(FText::FromString(CharName));

	UpdateHP();
	UpdateMP();
}

void UBattleCharInfoWidget::UpdateHP()
{
	if (!Attibute.IsValid())
		return;

	float OriginHP = Attibute.Get()->GetOriginHP();
	float CurrentHP = Attibute.Get()->GetHP();

	FString HPString = FString::Printf(TEXT("%.0f / %.0f"), CurrentHP, OriginHP);
	HPText->SetText(FText::FromString(HPString));

	float Percent = FMath::Clamp(CurrentHP / OriginHP, 0.0f, 1.0f);
	HPBar->SetPercent(Percent);
}

void UBattleCharInfoWidget::UpdateMP()
{
	if (!Attibute.IsValid())
		return;

	float OriginMP = Attibute.Get()->GetOriginSP();
	float CurrentMP = Attibute.Get()->GetSP();

	FString MPString = FString::Printf(TEXT("%.0f / %.0f"), CurrentMP, OriginMP);
	MPText->SetText(FText::FromString(MPString));

	float Percent = FMath::Clamp(CurrentMP / OriginMP, 0.0f, 1.0f);
	MPBar->SetPercent(Percent);
}
