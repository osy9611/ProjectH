// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericObjectPool.h"
#include "ModularGame/Log/ModularLogChannel.h"
#include "ModularGame/Pool/Poolable.h"

void UGenericObjectPool::Init(TFunction<UObject*(UWorld*)> InGenrator, int32 Count, TFunction<void(UObject*, bool)>InInitializer, TFunction<void(UObject*)>InDeInitializer)
{
	if (InGenrator)
		ObjectGenrator = InGenrator;

	if (InInitializer)
		Initializer = InInitializer;
	if (InDeInitializer)
		DeInitializer = InDeInitializer;

	if (!InGenrator)
	{
		UE_LOG(Modular, Error, TEXT("[GenricObjectPool] InGenrator is nullptr"));
		return;
	}

	for (int32 i = 0; i < Count; ++i)
	{
		UObject* Obj = ObjectGenrator(GetWorld());

		if (!Initializer)
		{
			Initializer(Obj, false);
		}

		ObjectPool.Add(Obj);
	}
}

UObject* UGenericObjectPool::Get(bool IsActive)
{
	UObject* NewObject = nullptr;

	if (ObjectPool.Num() > 0)
	{
		NewObject = ObjectPool.Pop();
	}
	else
	{
		NewObject = ObjectGenrator(GetWorld());
	}

	if (!Initializer)
		Initializer(NewObject, IsActive);

	if (IPoolable* Poolable = Cast<IPoolable>(NewObject))
	{
		Poolable->OnPoolGet();
	}

	ActiveObjects.Add(NewObject);

	return NewObject;
}

void UGenericObjectPool::Return(UObject* ReturnObject)
{
	if (!ReturnObject)
		return;

	ActiveObjects.Remove(ReturnObject);

	if (!DeInitializer)
		DeInitializer(ReturnObject);

	if (IPoolable* Poolable = Cast<IPoolable>(ReturnObject))
	{
		Poolable->OnPoolReturn();
	}

	ObjectPool.Add(ReturnObject);
}
