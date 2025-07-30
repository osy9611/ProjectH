// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SkillAction.h"
#include "Algo/Accumulate.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/Util/UtilFunc_AI.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/Battle/State/BattleState.h"

EBTNodeResult::Type UBTTaskNode_SkillAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (IsCompleted)
		return EBTNodeResult::Succeeded;

	Super::ExecuteTask(OwnerComp, NodeMemory);
	ActiveSkillAction();

	return EBTNodeResult::InProgress;
}

void UBTTaskNode_SkillAction::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	IsCompleted = false;
}

void UBTTaskNode_SkillAction::ActiveSkillAction()
{
	FGameplayTag ActionTag = GetSkillTag();
	if (!ActionTag.IsValid())
		return;

	FBattleStateParams Params;
	Params.SkillTag = ActionTag;
	Params.Objects = OnSelectTarget(ActionTag);
	Params.OnEndAbilityCallBack = [this]()
		{
			CompleteActiveSkill();
		};

	UtilFunc_AI::ExecuteSkill(GetWorld(), Params);

	IsCompleted = false;
}

void UBTTaskNode_SkillAction::CompleteActiveSkill()
{
	IsCompleted = true;
}

FGameplayTag UBTTaskNode_SkillAction::GetSkillTag()
{
	if (!BlackboardComp.IsValid())
		return FGameplayTag();

	FName SkillTagName = BlackboardComp->GetValueAsName("SkillActionTag");
	if (!SkillTagName.IsValid())
	{
		UE_LOG(HDLog, Log, TEXT("[BTTaskNode_SkillAction] SkillActionTag is Valid"));
		return FGameplayTag();
	}

	FGameplayTag SkillTag = FGameplayTag::RequestGameplayTag(SkillTagName);
	return SkillTag;
}

TArray<AActor*> UBTTaskNode_SkillAction::OnSelectTarget(FGameplayTag GameplayTag)
{
	TArray<AActor*> Result;
	UHDAttributeSet* AttributeSet = BattleComp->GetAttributeSet();

	FSkillData* SkillData = AttributeSet->GetSkillData(GameplayTag);
	if (!SkillData)
		return Result;

	//SelectTarget
	ECharType TargetType = BattleComp->CharType == ECharType::Character ? ECharType::Monster : ECharType::Character;

	switch (SkillData->SkillTargetType)
	{
	case ESkillTargetType::Mine:
		Result = { BattleComp->GetOwner() };
		break;
	case ESkillTargetType::TeamAll:
		Result = UtilFunc_AI::GetTarget(GetWorld(), BattleComp->CharType);
		break;
	case ESkillTargetType::Enemy_One:
		Result = SelectTarget(UtilFunc_AI::GetTarget(GetWorld(), TargetType), 1);
		break;
	case ESkillTargetType::Enemy_All:
		Result = UtilFunc_AI::GetTarget(GetWorld(), TargetType);
		break;
	case ESkillTargetType::Random:
		Result = SelectTargetRandom(UtilFunc_AI::GetTarget(GetWorld(), TargetType), SkillData->TargetCount);
		break;
	}


	return Result;
}

TArray<AActor*> UBTTaskNode_SkillAction::SelectTargetRandom(TArray<AActor*> TargetActor, int32 Count)
{
	TArray<AActor*> Result;
	TArray<AActor*> IndexArray;

	for (AActor* Actor : TargetActor)
	{
		IndexArray.Add(Actor);
	}

	for (int32 i = 0; i < IndexArray.Num(); ++i)
	{
		int32 j = FMath::RandRange(0, i);
		IndexArray.Swap(i, j);
	}

	for (int32 i = 0; i < IndexArray.Num(); ++i)
	{
		if (i <= Count)
		{
			Result.Add(IndexArray[i]);
		}
	}

	return Result;
}

//일반 타겟 설정법
//1.어그로
//2.HP 보너스
//3.기타 (추후 따로 구현)
TArray<AActor*> UBTTaskNode_SkillAction::SelectTarget(TArray<AActor*> TargetActor, int32 Count)
{
	TArray<float> TargetPers;
	for (AActor* Actor : TargetActor)
	{
		UHDBattleComponent* TargetBattleComp = UHDBattleComponent::FindBattleComponent(Actor);
		if (!TargetBattleComp)
			continue;

		UHDAttributeSet* AttributeSet = TargetBattleComp->GetAttributeSet();

		if (!AttributeSet)
			continue;

		AttributeSet->OnUpdateStatus();

		//1. Threat 추가
		float TargetPer = AttributeSet->GetThreat();

		//2. HP 보너스 추가
		float HPPer = AttributeSet->GetHP() / AttributeSet->GetOriginHP();
		if (HPPer < MiddleHPBonus)
			TargetPer += UtilFunc_Data::GetBattleConstValue(GetWorld(), EBattleConstType::HPMidBonus);
		else if (HPPer < MaxHPBonus)
			TargetPer += UtilFunc_Data::GetBattleConstValue(GetWorld(), EBattleConstType::HPMaxBonus);


		TargetPers.Add(TargetPer);
	}

	//1. 모든 확률의 합을 구하고
	int32 SumRate = Algo::Accumulate(TargetPers, 0.0f);

	//2. 총합이 1000을 못넘기면
	if (SumRate < 1000)
	{
		//3. 남은 확률을 구해서 각 확률에 분배해서 넣어준다.
		int32 RemainRate = 1000 - SumRate;
		int32 Value = RemainRate / TargetPers.Num();

		for (float& TargetPer : TargetPers)
		{
			TargetPer += Value;
		}
	}

	//4. 그래도 1000이 안되면
	SumRate = Algo::Accumulate(TargetPers, 0.0f);
	if (SumRate < 1000)
	{
		//5. 0번 인덱스에 남은 값을 몰아준다
		int32 RemainRate = 1000 - SumRate;
		if (TargetPers.Num() > 0)
		{
			TargetPers[0] += RemainRate;
		}
	}

	int32 RandomValue = UtilFunc_AI::RandomBattleSelect(GetWorld(), 0, 1000);

	int32 CurValue = 0;

	TArray<AActor*> Result;
	for (int32 i = 0; i < TargetPers.Num(); ++i)
	{
		CurValue += TargetPers[i];

		if (CurValue < RandomValue)
			continue;

		if (Result.Num() < Count)
		{
			Result.Add(TargetActor[i]);
		}
	}

	return Result;
}
