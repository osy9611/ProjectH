// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/Data/PlayerDataSubsystem.h"
#include "HDPlayerDataSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UHDPlayerDataSubsystem : public UPlayerDataSubsystem
{
	GENERATED_BODY()
public:
	virtual void RegisterUserData() override;
	virtual void RegisterListener() override;
};
