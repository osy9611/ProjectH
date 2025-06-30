// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Poolable_Actor.h"
#include "Poolable_NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "ModularGame/Log/ModularLogChannel.h"

UObjectPoolSubsystem::UObjectPoolSubsystem()
{
}

void UObjectPoolSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
}

void UObjectPoolSubsystem::CreatePool(const FName& PoolName, TSubclassOf<AActor> Actor, int32 Count)
{
	CreatePool(PoolName, [Actor](UWorld* World)
		{
			return World->SpawnActor<AActor>(Actor);
		},
		Count,
		[](UObject* Obj, bool IsActive)
		{
			if (!Obj)
				return;

			if (!IsActive)
				return;

			AActor* Actor = Cast<AActor>(Obj);
			Actor->SetActorHiddenInGame(false);
			Actor->SetActorEnableCollision(true);
			Actor->SetActorTickEnabled(true);
		},
		[](UObject* Obj)
		{
			AActor* Actor = Cast<AActor>(Obj);
			Actor->SetActorHiddenInGame(true);
			Actor->SetActorEnableCollision(false);
			Actor->SetActorTickEnabled(false);
		});
}

void UObjectPoolSubsystem::CreatePool(const FName& PoolName, TFunction<UObject* (UWorld*)> Generator, int32 Count, TFunction<void(UObject*, bool)>InInitializer, TFunction<void(UObject*)>InDeInitializer)
{
	if (Pools.Contains(PoolName))
	{
		UE_LOG(Modular, Warning, TEXT("[ObjectPoolSubsystem] Pool already exists: %s"), *PoolName.ToString());
		return;
	}

	UGenericObjectPool* NewPool = NewObject<UGenericObjectPool>(this);
	NewPool->Init(Generator, Count, InInitializer, InDeInitializer);
	Pools.Add(PoolName, NewPool);
}

UObject* UObjectPoolSubsystem::Get(FName PoolName, bool IsActive)
{
	if (UGenericObjectPool** Pool = Pools.Find(PoolName))
	{
		return (*Pool)->Get(IsActive);
	}

	return nullptr;
}

void UObjectPoolSubsystem::Return(FName PoolName, UObject* Obj)
{
	if (UGenericObjectPool** Pool = Pools.Find(PoolName))
	{
		(*Pool)->Return(Obj);
	}
}
