// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleState_MonsterTurn.h"
#include "ProjectH/Battle/AI/BattleAIController.h"
#include "ProjectH/Battle/Turn/TurnManager.h"

void UBattleState_MonsterTurn::Initailize()
{
	Super::Initailize();
}

void UBattleState_MonsterTurn::DoStart()
{
	Super::DoStart();

	if (!TurnManager.IsValid())
		return;

	UHDBattleComponent* BattleComp = TurnManager->GetCurrentActor();
	if (!BattleComp)
		return;

	ABattleAIController* Controller = BattleComp->GetController<ABattleAIController>();
	if (!Controller)
		return;

	CurrentAIController = Controller;
	CurrentAIController->OnStartBehavior();
}

void UBattleState_MonsterTurn::DoEnd()
{
	Super::DoEnd();
	if (!CurrentAIController.IsValid())
		return;

	CurrentAIController->OnEndBehavior();
	CurrentAIController = nullptr;
}

void UBattleState_MonsterTurn::DoExecute(FBattleStateParams& Params)
{
	UHDBattleComponent* BattleComp = TurnManager->GetCurrentActor();
	if (!BattleComp)
		return;

	if (BattleComp->CharType != ECharType::Monster)
		return;

	BattleComp->ProcessAbility_Skill(Params);
}

void UBattleState_MonsterTurn::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
}
