// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckPhase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CheckPhase::UBTDecorator_CheckPhase()
{
	NodeName = "Check Phase Equals";
	ExpectedPhase = 0;
}

bool UBTDecorator_CheckPhase::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
		return false;

	const int32 CurrentPhase = BB->GetValueAsInt(PhaseKey.SelectedKeyName);

	return (CurrentPhase == ExpectedPhase);
}
