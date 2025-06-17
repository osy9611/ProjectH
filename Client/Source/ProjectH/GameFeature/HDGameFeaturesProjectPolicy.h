// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeaturesProjectPolicies.h"
#include "HDGameFeaturesProjectPolicy.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UHDGameFeaturesProjectPolicy : public UDefaultGameFeaturesProjectPolicies
{
	GENERATED_BODY()
public:
	UHDGameFeaturesProjectPolicy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* GameFeaturesProjectPolicied interfases
	*/
	virtual void InitGameFeatureManager() override;
	virtual void ShutdownGameFeatureManager() override;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Observers;
};
