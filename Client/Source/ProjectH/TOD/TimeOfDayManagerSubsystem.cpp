// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeOfDayManagerSubsystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "ProjectH/LogChannels.h"

void UTimeOfDayManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTimeOfDayManagerSubsystem::Deinitialize()
{
	TODActor = nullptr;

	Super::Deinitialize();
}

void UTimeOfDayManagerSubsystem::Tick(float DeltaTime)
{
	if (!IsValid(TODActor))
		return;


}

void UTimeOfDayManagerSubsystem::RegisterTODActor(ATODActor* NewTODActor)
{
	if (!NewTODActor)
	{
		UE_LOG(HDLog, Log, TEXT("[TimeOfDayManagerSubsystem] TODActor Register Fail TODActor is nullptr"));
		return;
	}

	TODActor = NewTODActor;
}
