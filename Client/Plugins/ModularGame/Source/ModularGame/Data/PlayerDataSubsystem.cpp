// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDataSubsystem.h"
#include "ModularGame/Data/ModularUserData.h"
void UPlayerDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	RegisterUserData();
	RegisterListener();
}

void UPlayerDataSubsystem::Deinitialize()
{
	Super::Deinitialize();
}
