// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HDExperienceDefinition.generated.h"

class UHDPawnData;
class UHDExperienceActionSet;
class UGameFeatureAction;

UCLASS()
class PROJECTH_API UHDExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = GamePlay)
	TArray<FString> GameFeaturesToEnable;

	UPROPERTY(EditDefaultsOnly, Category = GamePlay)
	TArray<TObjectPtr<UHDExperienceActionSet>> ActionSets;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Actions")
	TArray<TObjectPtr<UGameFeatureAction>>Actions;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	TObjectPtr<UHDPawnData> DefaultPawnData;
};
