// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Base.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Util/UtilFunc_AI.h"
EBTNodeResult::Type UBTTaskNode_Base::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (BlackboardComp == nullptr)
	{
		BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (!BlackboardComp.IsValid())
		{
			UE_LOG(HDLog, Log, TEXT("[BTTaskNode_Base] BlackboardComp is nullptr"));
			return EBTNodeResult::Failed;
		}
	}

	if (BattleComp == nullptr)
	{
		BattleComp = UtilFunc_AI::GetBattleComponent(OwnerComp);
		if (!BattleComp.IsValid())
		{
			UE_LOG(HDLog, Log, TEXT("[BTTaskNode_Base] BlackboardComp is nullptr"));
			return EBTNodeResult::Failed;
		}
	}

	return EBTNodeResult::InProgress;
}
