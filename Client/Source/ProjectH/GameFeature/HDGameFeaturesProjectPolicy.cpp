// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameFeaturesProjectPolicy.h"

UHDGameFeaturesProjectPolicy::UHDGameFeaturesProjectPolicy(const FObjectInitializer& ObjectInitializer)
{
}

void UHDGameFeaturesProjectPolicy::InitGameFeatureManager()
{
	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.AddObserver(Observer);
	}

	Super::InitGameFeatureManager();
}

void UHDGameFeaturesProjectPolicy::ShutdownGameFeatureManager()
{
	Super::ShutdownGameFeatureManager();

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.RemoveObserver(Observer);
	}

	Observers.Empty();
}
