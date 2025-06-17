// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HDPawnData.generated.h"

class UHDInputConfig;
class UModularCameraMode;
class UHDAbilitySet;

UCLASS()
class PROJECTH_API UHDPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UHDPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TSubclassOf<APawn> PawnClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UModularCameraMode> DefaultCameraMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputConfig")
	TObjectPtr<UHDInputConfig> InputConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<UHDAbilitySet>> AbilitySets;
};
