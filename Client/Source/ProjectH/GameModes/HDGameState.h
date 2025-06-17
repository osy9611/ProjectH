// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HDGameState.generated.h"

class UHDExperienceManagerComponent;

UCLASS()
class PROJECTH_API AHDGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AHDGameState();

	UPROPERTY()
	TObjectPtr<UHDExperienceManagerComponent> ExperienceManagerComponent;
};
