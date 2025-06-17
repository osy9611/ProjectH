// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameState.h"
#include "ProjectH/GameModes/HDExperienceManagerComponent.h"

AHDGameState::AHDGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UHDExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
