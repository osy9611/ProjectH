// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDMappableConfigPair.generated.h"
class UPlayerMappableInputConfig;

USTRUCT()
struct FHDMappableConfigPair
{
	GENERATED_BODY()
public:
	FHDMappableConfigPair() = default;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	UPROPERTY(EditAnywhere)
	bool bShouldActivateAutomatically = true;
};