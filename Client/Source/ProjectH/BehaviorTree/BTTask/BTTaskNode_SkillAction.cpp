// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SkillAction.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/Util/UtilFunc_AI.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/Battle/State/BattleState.h"

EBTNodeResult::Type UBTTaskNode_SkillAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (IsCompleted)
		return EBTNodeResult::Succeeded;


	return Super::ExecuteTask(OwnerComp, NodeMemory);
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
		Result = UtilFunc_AI::GetTarget(GetWorld(), TargetType);
		break;
	case ESkillTargetType::Enemy_One:
		
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

TArray<AActor*> UBTTaskNode_SkillAction::SelectTarget(TArray<AActor*> TargetActor, int32 Count)
{
	return TArray<AActor*>();
}
