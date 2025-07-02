// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Battle/Data/BattleResourceData_AI.h"

ABattleAIController::ABattleAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

void ABattleAIController::OnInit(UBattleResourceData_AI* ResourceData)
{
	if (!ResourceData)
	{
		UE_LOG(HDLog, Log, TEXT("[BattleAIController] OnInit Fail ResourceData is nullptr"));
		return;
	}

	BehaviorTree = ResourceData->BehaviorTree;
	BlackboardAsset = ResourceData->BlackboardAsset;

	if (BlackboardAsset)
	{
		UseBlackboard(BlackboardAsset, BlackboardComp);
		//RunBehaviorTree(BehaviorTree);
	}
}

void ABattleAIController::OnStartBehavior()
{
	Blackboard->SetValueAsBool("IsOwnTurn", true);
	BehaviorTreeComp->StartTree(*BehaviorTree);
}

void ABattleAIController::OnEndBehavior()
{
	Blackboard->SetValueAsBool("IsOwnTurn", false);
	BehaviorTreeComp->StopTree();
}
