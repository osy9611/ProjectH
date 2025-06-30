// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/GameFeature/GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_BattleSetting.generated.h"

UCLASS()
class PROJECTH_API UGameFeatureAction_BattleSetting : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

	void SetPool(UWorld* World);
	void SetBattle(UWorld* World);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BattleChar")
	TSubclassOf<AActor> BattleChar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BattleAI")
	TSubclassOf<AActor> BattleAI;
};
