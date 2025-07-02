// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BattleAIController.generated.h"

class UHDBattleComponent;
class UBattleResourceData_AI;
class UBehaviorTreeComponent;

UCLASS()
class PROJECTH_API ABattleAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ABattleAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void OnInit(UBattleResourceData_AI* ResourceData);

	void OnStartBehavior();
	void OnEndBehavior();
public:
	UPROPERTY()
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY()
	TObjectPtr<UBlackboardData> BlackboardAsset;
	UPROPERTY()
	UBlackboardComponent* BlackboardComp;

	TWeakPtr<UHDBattleComponent> BattleComponent;
};
