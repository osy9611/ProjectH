// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ToggleData.generated.h"

/**
 * 
 */
UCLASS()
class MODULARGAME_API UToggleData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	bool IsToggled;

	UPROPERTY()
	FString UniqueID;
};
