// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/Object/HDObjectPoolSubsystem.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UParticleSystemComponent;
class UParticleSystem;

class PROJECTH_API UtilFunc_Pooling
{
public:
	template<typename T = UObject>
	static T* Get(UWorld* World, FName PoolName, bool IsActive);

	template<typename T = UObject>
	static bool Return(UWorld* World, FName PoolName, T* TargetObject);
};

template<typename T>
T* UtilFunc_Pooling::Get(UWorld* World, FName PoolName, bool IsActive)
{
	if (!World)
		return nullptr;

	UHDObjectPoolSubsystem* PoolSubsystem = World->GetSubsystem<UHDObjectPoolSubsystem>();
	if (!PoolSubsystem)
		return nullptr;

	return PoolSubsystem->Get<T>(PoolName, IsActive);
}

template<typename T>
inline bool UtilFunc_Pooling::Return(UWorld* World, FName PoolName, T* TargetObject)
{
	if (!World)
		return false;

	UHDObjectPoolSubsystem* PoolSubsystem = World->GetSubsystem<UHDObjectPoolSubsystem>();
	if (!PoolSubsystem)
		return false;

	PoolSubsystem->Return(PoolName, TargetObject);

	return true;
}
