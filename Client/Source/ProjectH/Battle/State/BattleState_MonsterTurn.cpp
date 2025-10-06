// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleState_MonsterTurn.h"
#include "ProjectH/Util/UtilFunc.h"
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

	UHDAbilitySystemComponent* ASC = UtilFunc::GetASC(BattleComp->GetOwningActor());
	if (ASC)
		ASC->UpdateBuffTurns();

	CurrentAIController = Controller;
	CurrentAIController->OnStartBehavior();
}

void UBattleState_MonsterTurn::DoEnd()
{
	if (!CurrentAIController.IsValid())
		return;

	CurrentAIController->OnEndBehavior();
	CurrentAIController = nullptr;


	Super::DoEnd();
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

void UBattleState_MonsterTurn::HandleEndSequence(TFunction<void()> Callback)
{
	SequenceEndCallback = Callback;
	Callback();
}

void UBattleState_MonsterTurn::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
}
