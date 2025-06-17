// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BattleResourceData_AI.generated.h"

class UHDAbilitySet;
class UPaperFlipbook;
class UPaperZDAnimInstance;
class UPaperSprite;
class UBehaviorTree;
class UBlackboardData;

UCLASS()
class PROJECTH_API UBattleResourceData_AI : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UBattleResourceData_AI(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimInstance")
	TSubclassOf<UPaperZDAnimInstance> AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PaperFlipbook")
	TObjectPtr<UPaperFlipbook> PaperFilpbook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PaperFlipbook")
	TObjectPtr<UPaperSprite> BattleTurnSlotIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<UHDAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBlackboardData> BlackboardAsset;
};
