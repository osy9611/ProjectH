// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckPhase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UBTDecorator_CheckPhase : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_CheckPhase();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
public:

	//현재 Phase를 가져올 Blackboard Key
	UPROPERTY(EditAnywhere, Category = "Condition")
	FBlackboardKeySelector PhaseKey;

	//이 Decorator가 허용할 Phase 번호
	UPROPERTY(EditAnywhere, Category = "Condition")
	int32 ExpectedPhase;
};
