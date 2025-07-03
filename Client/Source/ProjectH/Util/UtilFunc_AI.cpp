// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilFunc_AI.h"
#include "AIController.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Battle/BattleSubsystem.h"
UHDBattleComponent* UtilFunc_AI::GetBattleComponent(UBehaviorTreeComponent& Comp)
{
    AAIController* Controller = Comp.GetAIOwner();
    if (!Controller)
        return nullptr;

    APawn* Pawn = Controller->GetPawn();
    if (!Pawn)
        return nullptr;

    UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(Pawn);
    return BattleComp;
}

int32 UtilFunc_AI::RandomSkillSelect(UWorld* World,int32 Min, int32 Max)
{
    UBattleSubsystem* BattleSubsystem = World->GetSubsystem<UBattleSubsystem>();
    if (!BattleSubsystem)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc_AI] BattleSubsystem is nullptr"));
        return int32();
    }

    return BattleSubsystem->RandomSkillSelect(Min, Max);
}

void UtilFunc_AI::ExecuteSkill(UWorld* World, FBattleStateParams& Params)
{
    UBattleSubsystem* BattleSubsystem = World->GetSubsystem<UBattleSubsystem>();
    if (!BattleSubsystem)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc_AI] BattleSubsystem is nullptr"));
        return;
    }
    BattleSubsystem->HandleAttackExecute(Params);
}
