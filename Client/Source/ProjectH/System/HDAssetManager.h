// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/Asset/ModularAssetManager.h"
#include "HDAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UHDAssetManager : public UModularAssetManager
{
	GENERATED_BODY()
public:
	static UHDAssetManager& Get();

	/*
	* UAssetManager's interfaces
	*/
	virtual void StartInitialLoading() final;
};
