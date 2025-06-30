// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ModularGame/Pool/GenericObjectPool.h"
#include "ObjectPoolSubsystem.generated.h"

class UPoolable_Actor;
class UPoolable_NiagaraSystem;
class UNiagaraSystem;
class UNiagaraComponent;
class UGenericObjectPool;

UCLASS()
class MODULARGAME_API UObjectPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UObjectPoolSubsystem();

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION(BlueprintCallable)
	virtual void CreatePool(const FName& PoolName, TSubclassOf<AActor> Actor, int32 Count);

	virtual void CreatePool(const FName& PoolName, TFunction<UObject* (UWorld*)>Generator, int32 Count, TFunction<void(UObject*, bool)>InInitializer = nullptr,
					TFunction<void(UObject*)>InDeInitializer = nullptr);

	UFUNCTION(BlueprintCallable)
	UObject* Get(FName PoolName,bool IsActive);

	template<typename T = UObject>
	T* Get(FName PoolName, bool IsActive);

	UFUNCTION(BlueprintCallable)
	void Return(FName PoolName, UObject* Obj);

protected:
	void CreatePoolRoot();

protected:
	UPROPERTY()
	TObjectPtr<AActor>SpawnOwnerActor;

	UPROPERTY()
	TMap<FName, UGenericObjectPool*> Pools;
};

template<typename T>
T* UObjectPoolSubsystem::Get(FName PoolName, bool IsActive)
{
	if (UGenericObjectPool** Pool = Pools.Find(PoolName))
	{
		UObject* Result = (*Pool)->Get(IsActive);
		return Cast<T>(Result);
	}
	return nullptr;
}
