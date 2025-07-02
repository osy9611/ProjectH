// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/LogChannels.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

class AAIController;
class UBehaviorTreeComponent;
class UHDBattleComponent;

class PROJECTH_API UtlFunc_AI
{
public:
	template<typename T = AAIController>
	T* GetAIController(UBehaviorTreeComponent& OwnerComp);
};

template<typename T>
T* UtlFunc_AI::GetAIController(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		UE_LOG(HDLog, Warning, TEXT("[UtilFunc_AI] GetAIController Fail"));
		return nullptr;
	}	

	return Cast<T>(Controller);
}
