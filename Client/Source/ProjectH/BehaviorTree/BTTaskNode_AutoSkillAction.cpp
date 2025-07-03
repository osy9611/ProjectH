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

	//1. ��� Ȯ���� ���� ���ϰ�
	int32 SumRate = SumValue();

	//2. ������ 1000�� ���ѱ��
	if (SumRate < 1000)
	{
		//3. ���� Ȯ���� ���ؼ� �� Ȯ���� �й��ؼ� �־��ش�.
		int32 RemainRate = 1000 - SumRate;
		int32 Value = RemainRate / SkillInfos.Num();

		for (FAutoSkillInfo& SkillInfo : SkillInfos)
		{
			SkillInfo.SkillRate += Value;
		}
	}

	//4. �׷��� 1000�� �ȵǸ� (int�� ������ ������ ���� �սǵǾ� 1000�� �ȵ� �� �ִ�.)
	SumRate = SumValue();
	if (SumRate < 1000)
	{
		//5. 0�� �ε����� ���� ���� �����ش�. (��ų1 �⺻ ���� ��ų�� ������)
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
