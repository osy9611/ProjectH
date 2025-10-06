// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/BehaviorTree/BTTask/BTTaskNode_Base.h"
#include "BTTaskNode_SkillAction.generated.h"

UCLASS()
class PROJECTH_API UBTTaskNode_SkillAction : public UBTTaskNode_Base
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	void ActiveSkillAction(UBehaviorTreeComponent& OwnerComp);
	void CompleteActiveSkill();
	FGameplayTag GetSkillTag();

	//Select Target
	TArray<AActor*> OnSelectTarget(FGameplayTag GameplayTag);

	TArray<AActor*> SelectTargetRandom(TArray<AActor*> TargetActor,int32 Count);
	TArray<AActor*> SelectTarget(TArray<AActor*> TargetActor,int32 Count);

public:
	UPROPERTY(EditAnywhere, Category = "HPBonus")
	float MiddleHPBonus = 0.3f;
	UPROPERTY(EditAnywhere, Category = "HPBonus")
	float MaxHPBonus = 0.6f;

private:
	UPROPERTY(EditAnywhere, Category = "SkillAction")
	bool IsCompleted = false;
};
