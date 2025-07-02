// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TurnEnd.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Battle/BattleSubsystem.h"
UBTTaskNode_TurnEnd::UBTTaskNode_TurnEnd()
{
}

EBTNodeResult::Type UBTTaskNode_TurnEnd::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBattleSubsystem* BattleSubsystem = GetWorld()->GetSubsystem<UBattleSubsystem>();
	if (!BattleSubsystem)
	{
		UE_LOG(HDLog, Error, TEXT("[UBTTaskNode_TurnEnd] BattleSubsystem is nullptr"));
		return EBTNodeResult::Failed;
	}

	BattleSubsystem->CheckBattleState();

	return EBTNodeResult::Succeeded;
}
