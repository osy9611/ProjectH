// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ProjectH/BehaviorTree/BTTask/BTTaskNode_Base.h"
#include "BTTaskNode_CheckDeath.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UBTTaskNode_CheckDeath : public UBTTaskNode_Base
{
	GENERATED_BODY()
public:
	UBTTaskNode_CheckDeath();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
