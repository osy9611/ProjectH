// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_CheckDeath.h"
#include "AIController.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Util/UtilFunc_AI.h"
#include "ProjectH/Battle/HDBattleComponent.h"

UBTTaskNode_CheckDeath::UBTTaskNode_CheckDeath()
{
}

EBTNodeResult::Type UBTTaskNode_CheckDeath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!BattleComp.IsValid())
		return EBTNodeResult::Failed;

	if (BattleComp->CheckDead())
	{
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Succeeded;
}
