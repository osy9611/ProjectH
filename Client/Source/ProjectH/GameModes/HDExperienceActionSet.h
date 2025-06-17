// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HDExperienceActionSet.generated.h"

class UGameFeatureAction;

UCLASS()
class PROJECTH_API UHDExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UHDExperienceActionSet();

	UPROPERTY(EditAnywhere, Category = "Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};
