// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_CheckDeath.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UBTTaskNode_CheckDeath : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskNode_CheckDeath();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
