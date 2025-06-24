// Fill out your copyright notice in the Description page of Project Settings.


#include "HDAttributeSet_Monster.h"
#include "GameplayEffectExtension.h"
#include "Components/WidgetComponent.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/UI/Battle/BattleMonsterInfoWidget.h"
#include "ProjectH/UI/Damage/HDDamageWidget.h"

UHDAttributeSet_Monster::UHDAttributeSet_Monster(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UHDAttributeSet_Monster::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	AActor* Actor = GetOwningActor();
	if (Data.EvaluatedData.Attribute == GetHPAttribute())
	{
		float NowHP = GetHP();
		UE_LOG(HDLog, Log, TEXT("[HDAttributeSet_Monster] Now HP %.2f"), NowHP);

		UWidgetComponent* InfoWidgetComp = UtilFunc::GetActorComponent<UWidgetComponent>(Actor, "InfoWidget");
		UUserWidget* InfoWidget = InfoWidgetComp->GetUserWidgetObject();
		if (InfoWidgetComp)
		{
			if (UBattleMonsterInfoWidget* MonsterInfoWidget = Cast<UBattleMonsterInfoWidget>(InfoWidget))
			{
				MonsterInfoWidget->UpdateHP(Actor);
			}
		}

		UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(Actor);
		if (!BattleComp)
			return;

		UBattleMonsterInfoWidget* MonsterInfoWidget = BattleComp->GetWidgetObject<UBattleMonsterInfoWidget>("InfoWidget", true);
		if (MonsterInfoWidget)
			MonsterInfoWidget->UpdateHP(Actor);

		UHDDamageWidget* DamageWidget = BattleComp->GetWidgetObject<UHDDamageWidget>("DamageWidget", true);
		if (DamageWidget)
		{
			DamageWidget->StartDamageWidget(Data.EvaluatedData.Magnitude);
		}
	}
}

void UHDAttributeSet_Monster::OnInit(FString InitTableID)
{
	Super::OnInit(InitTableID);

	OnUpdateStatus();

	FMonsterData* MonsterData = UtilFunc_Data::GetTableData<FMonsterData>(GetWorld(), TableID);
	if (!MonsterData)
		return;

	SetSkillData(MonsterData->SkillNos);
}

void UHDAttributeSet_Monster::OnUpdateStatus()
{
	FMonsterData* MonsterData = UtilFunc_Data::GetTableData<FMonsterData>(GetWorld(), TableID);

	if (!MonsterData)
		return;

	FMonsterStatusData* StatusData = UtilFunc_Data::GetTableData<FMonsterStatusData>(GetWorld(), FString::FromInt(MonsterData->StatusNo));
	if (!StatusData)
		return;

	OriginHP = StatusData->HP;
	HP = StatusData->HP;
	PATK = StatusData->PATK;
	PDEF = StatusData->PDEF;
	EATK = StatusData->EATK;
	EDEF = StatusData->EDEF;
	CRI = StatusData->CRI;
	Speed = StatusData->Speed;
}

