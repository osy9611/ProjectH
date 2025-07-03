// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_AutoSkillAction.h"
#include "ProjectH/Util/UtilFunc_AI.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Battle/State/BattleState.h"
UBTTaskNode_AutoSkillAction::UBTTaskNode_AutoSkillAction()
{
}

EBTNodeResult::Type UBTTaskNode_AutoSkillAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UHDBattleComponent* BattleComp = UtilFunc_AI::GetBattleComponent(OwnerComp);
	if (!BattleComp)
		return EBTNodeResult::Failed;

	if (IsEndSkill)
		EBTNodeResult::Succeeded;

	if (!IsActiveSkill)
	{
		ActivateSkillAction(BattleComp);
	}

	return EBTNodeResult::InProgress;
}

void UBTTaskNode_AutoSkillAction::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	IsEndSkill = false;
	IsActiveSkill = false;
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTaskNode_AutoSkillAction::ActivateSkillAction(UHDBattleComponent* BattleComp)
{
	FGameplayTag ActionTag = SelectAction();
	if (!ActionTag.IsValid())
		return;

	FBattleStateParams Params;
	Params.SkillTag = ActionTag;
	Params.OnEndAbilityCallBack = [this]()
		{
			OnCompleteSkillAction();
		};

	UtilFunc_AI::ExecuteSkill(GetWorld(), Params);

	IsActiveSkill = true;
}

FGameplayTag UBTTaskNode_AutoSkillAction::SelectAction()
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

		for (FAutoSkillInfo& SkillInfo : SkillInfos)
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
	for (FAutoSkillInfo& SkillInfo : SkillInfos)
	{
		CurValue += SkillInfo.SkillRate;
		if (CurValue < RandomValue)
			continue;

		return SkillInfo.SkillTag;
	}

	return FGameplayTag();
}

int32 UBTTaskNode_AutoSkillAction::SumValue()
{
	int32 Result = 0;
	for (FAutoSkillInfo& SkillInfo : SkillInfos)
	{
		Result += SkillInfo.SkillRate;
	}
	return Result;
}

void UBTTaskNode_AutoSkillAction::OnCompleteSkillAction()
{
	IsEndSkill = true;
}
