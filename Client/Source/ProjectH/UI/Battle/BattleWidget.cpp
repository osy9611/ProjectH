// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleWidget.h"
#include "Components/VerticalBox.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/UI/Battle/BattleTurnWidget.h"
#include "ProjectH/UI/Battle/BattleCharInfoWidget.h"
#include "ProjectH/UI/Battle/BattleCharActiveWidget.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/Battle/HDBattleComponent.h"

UBattleWidget::UBattleWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UBattleWidget::OnInit()
{
	InitCharInfoWidget();
	InitBattleTurnWidget();
	InitActiveBattleCharActiveWidget();
}

void UBattleWidget::InitCharInfoWidget()
{
	int32 ChildCount = CharInfoRoot->GetChildrenCount();

	for (int32 i = 0; i < ChildCount; ++i)
	{
		UWidget* Widget = CharInfoRoot->GetChildAt(i);
		if (!Widget)
			continue;

		UBattleCharInfoWidget* CharInfoWidget = Cast<UBattleCharInfoWidget>(Widget);
		if (!CharInfoWidget)
			continue;

		CharInfoWidget->SetVisibility(ESlateVisibility::Hidden);		
		CharInfos.Add(CharInfoWidget);
	}

	UpdateCharInfoWidget();
}

void UBattleWidget::ResetCharInfoWidget()
{
	for (UBattleCharInfoWidget* CharInfo : CharInfos)
	{
		CharInfo->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBattleWidget::UpdateCharInfoWidget()
{
	UBattleSubsystem* BattleSubsystem = GetWorld()->GetSubsystem<UBattleSubsystem>();
	if (!BattleSubsystem)
	{
		UE_LOG(HDLog, Error, TEXT("[BattleWidget] BattleSubsystem is nullptr"));
		return;
	}

	TArray<AActor*> BattleActors = BattleSubsystem->GetBattleActors();
	int32 Index = 0;
	for (const AActor* Actor : BattleActors)
	{
		if (!IsValid(Actor))
			continue;

		UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(Actor);
		if (!BattleComp)
			continue;

		CharInfos[Index]->OnInit(BattleComp->GetAttributeSet());
		CharInfos[Index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Index++;
	}
}

void UBattleWidget::InitBattleTurnWidget()
{
	BattleTurn->OnInit();
}

void UBattleWidget::UpdateBattleTurnWidget()
{
	BattleTurn->UpdateSlots();
}
void UBattleWidget::InitActiveBattleCharActiveWidget()
{
	BattleCharActive->SetVisibility(ESlateVisibility::Hidden);
}

void UBattleWidget::UpdateBattleCharActiveWidget(const FCharacterData& CharacterData)
{
	BattleCharActive->UpdateToggles(CharacterData);
}


void UBattleWidget::ActiveBattleCharActiveWidget()
{
	BattleCharActive->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UBattleWidget::DeActiveBattleCharActiveWidget()
{
	BattleCharActive->SetVisibility(ESlateVisibility::Hidden);
}
