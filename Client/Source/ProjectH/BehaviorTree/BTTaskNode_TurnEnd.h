// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_TurnEnd.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UBTTaskNode_TurnEnd : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskNode_TurnEnd();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
