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

	//���� Phase�� ������ Blackboard Key
	UPROPERTY(EditAnywhere, Category = "Condition")
	FBlackboardKeySelector PhaseKey;

	//�� Decorator�� ����� Phase ��ȣ
	UPROPERTY(EditAnywhere, Category = "Condition")
	int32 ExpectedPhase;
};
