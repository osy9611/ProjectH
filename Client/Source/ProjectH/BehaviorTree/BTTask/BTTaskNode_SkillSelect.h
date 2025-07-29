// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/BehaviorTree/BTTask/BTTaskNode_Base.h"
#include "BTTaskNode_SkillSelect.generated.h"

USTRUCT(BlueprintType)
struct FBTSkillInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SkillTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkillRate;
};

UCLASS()
class PROJECTH_API UBTTaskNode_SkillSelect : public UBTTaskNode_Base
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	FGameplayTag SelectAction();
	int32 SumValue();
public:
	UPROPERTY(EditAnywhere, Category = "SkillInfo")
	TArray<FBTSkillInfo> SkillInfos;

private:
	bool IsSelected = false;
};
