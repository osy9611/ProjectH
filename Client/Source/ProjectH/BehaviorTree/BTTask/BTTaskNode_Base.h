// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Base.generated.h"

class UBlackboardComponent;
class UHDBattleComponent;
UCLASS()
class PROJECTH_API UBTTaskNode_Base : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	UPROPERTY()
	TWeakObjectPtr<UBlackboardComponent> BlackboardComp;

	UPROPERTY()
	TWeakObjectPtr<UHDBattleComponent> BattleComp;
};
