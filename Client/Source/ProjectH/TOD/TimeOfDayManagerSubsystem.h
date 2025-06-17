// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProjectH/TOD/TODActor.h"
#include "TimeOfDayManagerSubsystem.generated.h"

class UGameplayMessageSubsystem;

UCLASS()
class PROJECTH_API UTimeOfDayManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return IsValid(TODActor); }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UTimeOfDaySubsystem, STATGROUP_Tickables); }
	
	void RegisterTODActor(ATODActor* NewTODActor);
private:
	UPROPERTY()
	TObjectPtr<ATODActor> TODActor;

	float CurrentTime = 0.0f;

	float DayLengthSeconds = 1200.f;
};
