// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/LogChannels.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

class AAIController;
class UBehaviorTreeComponent;
class UHDBattleComponent;
struct FBattleStateParams;

class PROJECTH_API UtilFunc_AI
{
public:
	template<typename T = AAIController>
	static T* GetAIController(UBehaviorTreeComponent& OwnerComp);

	static UHDBattleComponent* GetBattleComponent(UBehaviorTreeComponent& Comp);

	static int32 RandomSkillSelect(UWorld* World,int32 Min, int32 Max);

	static void ExecuteSkill(UWorld* World, FBattleStateParams& Params);
};

template<typename T>
T* UtilFunc_AI::GetAIController(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		UE_LOG(HDLog, Warning, TEXT("[UtilFunc_AI] GetAIController Fail"));
		return nullptr;
	}	

	return Cast<T>(Controller);
}
