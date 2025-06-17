// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddWidgets.generated.h"

class UCommonUserWidget;
class UCommonActivatableWidget;

UCLASS()
class PROJECTH_API UGameFeatureAction_AddWidgets : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

	void AddWidgets(UWorld* World);
	void RemoveWidgets();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TMap<FGameplayTag, TSubclassOf<UCommonActivatableWidget>> CreateWidgets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UCommonUserWidget> OverlayWidget;
};
