// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SkillSelect.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectH/Util/UtilFunc_AI.h"

EBTNodeResult::Type UBTTaskNode_SkillSelect::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	if (!IsSelected)
	{
		FGameplayTag SkillActionTag = SelectAction();

		if (SkillActionTag.IsValid())
		{
			BlackboardComp->SetValueAsName("SkillActionTag", SkillActionTag.GetTagName());
			IsSelected = true;

			return EBTNodeResult::Succeeded;
		}

		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}

void UBTTaskNode_SkillSelect::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	IsSelected = false;
}

FGameplayTag UBTTaskNode_SkillSelect::SelectAction()
{
	if(SkillInfos.IsEmpty())
		return FGameplayTag();

	//1. 모든 확률의 합을 구하고
	int32 SumRate = SumValue();

	//2. 총합이 1000을 못넘기면
	if (SumRate < 1000)
	{
		//3. 남은 확률을 구해서 각 확률에 분배해서 넣어준다.
		int32 RemainRate = 1000 - SumRate;
		int32 Value = RemainRate / SkillInfos.Num();

		for (FBTSkillInfo& SkillInfo : SkillInfos)
		{
			SkillInfo.SkillRate += Value;
		}
	}

	//4. 그래도 1000이 안되면 (int로 나누기 때문에 값이 손실되어 1000이 안될 수 있다.)
	SumRate = SumValue();
	if (SumRate < 1000)
	{
		//5. 0번 인덱스에 남은 값을 몰아준다. (스킬1 기본 공격 스킬에 몰아줌)
		int32 RemainRate = 1000 - SumRate;
		if (SkillInfos.Num() > 0)
		{
			SkillInfos[0].SkillRate += RemainRate;
		}
	}

	int32 RandomValue = UtilFunc_AI::RandomSkillSelect(GetWorld(), 0, 1000);

	int32 CurValue = 0;
	for (FBTSkillInfo& SkillInfo : SkillInfos)
	{
		CurValue += SkillInfo.SkillRate;
		if (CurValue < RandomValue)
			continue;

		return SkillInfo.SkillTag;
	}

	return FGameplayTag();
}

int32 UBTTaskNode_SkillSelect::SumValue()
{
	int32 Result = 0;
	for (FBTSkillInfo& SkillInfo : SkillInfos)
	{
		Result += SkillInfo.SkillRate;
	}
	return Result;
}
