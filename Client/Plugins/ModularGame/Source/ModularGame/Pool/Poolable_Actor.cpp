// Fill out your copyright notice in the Description page of Project Settings.


#include "Poolable_Actor.h"
#include "ModularGame/Log/ModularLogChannel.h"

void UPoolable_Actor::Init(UWorld* World, TSubclassOf<AActor> Actor, int32 Count)
{
	if (!Actor)
	{
		UE_LOG(Modular, Log, TEXT("Actor Not Found"));
	}
	OriginalActor = Actor;
	for (int32 i = 0; i < Count; ++i)
	{
		if (AActor* NewActor = Create(World))
		{
			HideActor(NewActor);
			ObjectPool.Add(NewActor);
		}
	}
}

AActor* UPoolable_Actor::Get(UWorld* World, bool IsActive)
{
	AActor* Result = nullptr;
	if (ObjectPool.IsEmpty())
		Result = Create(World);
	else
		Result = ObjectPool.Pop();

	if (Result && IsActive)
		ShowActor(Result);

	return Result;
}

void UPoolable_Actor::Return(AActor* Actor)
{
	check(Actor);

	HideActor(Actor);
	ObjectPool.Add(Actor);
}

IPoolableActor* UPoolable_Actor::GetInterface(AActor* Actor)
{
	check(Actor);

	if (Actor->GetClass()->ImplementsInterface(UPoolableActor::StaticClass()))
	{
		return Cast<IPoolableActor>(Actor);
	}

	//if (Actor->Implements<IPoolableActor>())
	//{
	//	/*void* InterfaceAddress = Actor->GetInterfaceAddress(UPoolableActor::StaticClass());
	//	if (InterfaceAddress)
	//	{
	//		return static_cast<IPoolableActor*>(InterfaceAddress);
	//	}*/
	//}

	return nullptr;
}

void UPoolable_Actor::HideActor(AActor* Actor)
{
	check(Actor);

	IPoolableActor* Interface = GetInterface(Actor);
	if (Interface)
	{
		Interface->OnDeActive();

		IPoolableActor::Execute_K2_OnDeActive(Actor);
	}

	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);
}

void UPoolable_Actor::ShowActor(AActor* Actor)
{
	check(Actor);

	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);
	Actor->SetActorTickEnabled(true);

	IPoolableActor* Interface = GetInterface(Actor);
	if (Interface)
	{
		Interface->OnActive();
		IPoolableActor::Execute_K2_OnActive(Actor);
	}
}
