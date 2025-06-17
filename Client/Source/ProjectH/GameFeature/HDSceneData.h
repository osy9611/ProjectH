// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CommonSessionSubsystem.h"
#include "HDSceneData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UHDSceneData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	UMoveSessionData* CreateMoveSessionData(int32 TableNo);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "HDExperienceDefinition"))
	FPrimaryAssetId ExperinceID;
};
