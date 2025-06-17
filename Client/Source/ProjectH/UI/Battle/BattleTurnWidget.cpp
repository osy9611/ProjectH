// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTurnWidget.h"
#include "Components/HorizontalBox.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/UI/Battle/BattleTurnSlot.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/Battle/Turn/TurnManager.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Battle/Data/BattleResourceData_AI.h"
#include "ProjectH/Battle/Data/BattleResourceData_Player.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet_Monster.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet_Player.h"

UBattleTurnWidget::UBattleTurnWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UBattleTurnWidget::OnInit()
{
	UBattleSubsystem* BattleSubsystem = GetWorld()->GetSubsystem<UBattleSubsystem>();
	if (!BattleSubsystem)
	{
		UE_LOG(HDLog, Error, TEXT("[BattleTurnWidget] BattleSubsystem is nullptr"));
		return;
	}

	TurnManager = BattleSubsystem->GetTurnManager();

	GetTurnSlots();
	CheckCreateSlot();
}

void UBattleTurnWidget::GetTurnSlots()
{
	int32 ChildCount = TurnSlotRoot->GetChildrenCount();

	if (TurnSlots.IsEmpty())
	{
		for (int i = 0; i < ChildCount; ++i)
		{
			UWidget* Widget = TurnSlotRoot->GetChildAt(i);
			if (!Widget)
				continue;

			UBattleTurnSlot* TurnSlot = Cast<UBattleTurnSlot>(Widget);
			if (!TurnSlot)
				continue;
			TurnSlot->SetVisibility(ESlateVisibility::Hidden);
			TurnSlots.Add(TurnSlot);
		}
	}
}

void UBattleTurnWidget::CheckCreateSlot()
{

}

void UBattleTurnWidget::UpdateSlots()
{
	if (!TurnManager.IsValid())
		return;
	const TArray<FTimelineEntry> Timeline = TurnManager.Get()->GetTimeline();

	int32 SlotIndex = 0;
	for (const FTimelineEntry& Entry : Timeline)
	{
		UHDBattleComponent* BattleComp = Entry.BattleComp;

		if (!BattleComp)
			continue;

		if (BattleComp->CharType == ECharType::Character)
		{
			UpdateSlot_Char(SlotIndex, BattleComp);
		}

		if (BattleComp->CharType == ECharType::Monster)
		{
			UpdateSlot_Monster(SlotIndex, BattleComp);
		}

		TurnSlots[SlotIndex]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SlotIndex++;
	}
}

void UBattleTurnWidget::UpdateSlot_Char(int32 Index, UHDBattleComponent* BattleComp)
{
	UHDAttributeSet_Player* Player = Cast<UHDAttributeSet_Player>(BattleComp->GetAttributeSet());

	FCharacterData* CharData = Player->GetTableData<FCharacterData>();

	if (!CharData)
		return;

	UBattleResourceData_Player* ResourceData = UtilFunc::LoadResource<UBattleResourceData_Player>(CharData->BattleResource);
	if (!ResourceData)
		return;
	
	TurnSlots[Index]->OnInit(ResourceData->BattleTurnSlotIcon);
}

void UBattleTurnWidget::UpdateSlot_Monster(int32 Index, UHDBattleComponent* BattleComp)
{
	UHDAttributeSet_Monster* Monster = Cast<UHDAttributeSet_Monster>(BattleComp->GetAttributeSet());
	FMonsterData* MonsterData = Monster->GetTableData<FMonsterData>();

	if (!MonsterData)
		return;

	UBattleResourceData_AI* ResourceData = UtilFunc::LoadResource<UBattleResourceData_AI>(MonsterData->ResourcePath);
	if (!ResourceData)
		return;

	TurnSlots[Index]->OnInit(ResourceData->BattleTurnSlotIcon);
}
