// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleState_Play.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/UI/Battle/BattleWidget.h"
#include "ProjectH/UI/HDUIManagerSubsystem.h"
#include "ProjectH/Battle/Turn/TurnManager.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
void UBattleState_Play::Initailize()
{
	Super::Initailize();
}

void UBattleState_Play::DoStart()
{
	Super::DoStart();

	//Update Turn Widget
	BattleWidget->UpdateBattleTurnWidget();
	TurnManager->ExecuteNextTurn();

	UHDBattleComponent* CurrentActor = TurnManager->GetCurrentActor();
	if (!CurrentActor)
		return;

	TArray<FSkillData> SkillDatas = CurrentActor->GetAttributeSet()->GetSkillDatas();
	BattleWidget->UpdateBattleCharActiveWidget(SkillDatas);
}

void UBattleState_Play::DoEnd()
{
	Super::DoEnd();
}

void UBattleState_Play::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
}
