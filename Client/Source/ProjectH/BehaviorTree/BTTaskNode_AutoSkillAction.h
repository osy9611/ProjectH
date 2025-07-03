// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_AutoSkillAction.generated.h"

class UHDBattleComponent;

USTRUCT(BlueprintType)
struct FAutoSkillInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SkillTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkillRate;
};

UCLASS()
class PROJECTH_API UBTTaskNode_AutoSkillAction : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_AutoSkillAction();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	void ActivateSkillAction(UHDBattleComponent* BattleComp);
	FGameplayTag SelectAction();
	int32 SumValue();
	void OnCompleteSkillAction();

public:
	UPROPERTY(EditAnywhere, Category = "SkillInfo");
	TArray<FAutoSkillInfo> SkillInfos;

	UPROPERTY(EditDefaultsOnly, Category = "SkillInfo")
	bool IsActiveSkill = false;

	bool IsEndSkill = false;
};
