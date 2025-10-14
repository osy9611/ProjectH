// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Poolable_Actor.generated.h"

UINTERFACE(MinimalAPI)
class UPoolableActor : public UInterface
{
	GENERATED_BODY()
};

class MODULARGAME_API IPoolableActor
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnActive();

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnDeActive();

	virtual void OnActive() = 0;
	virtual void OnDeActive() = 0;
};

/**
 * 
 */
UCLASS()
class MODULARGAME_API UPoolable_Actor : public UObject
{
	GENERATED_BODY()
public:
	bool IsEmpty() { return ObjectPool.IsEmpty(); }
	int32 GetCount() { return ObjectPool.Num(); }

	void Init(UWorld* World, TSubclassOf<AActor> Actor, int32 Count = 5);

	AActor* Get(UWorld* World,bool IsActive);
	void Return(AActor* Actor);

	AActor* Create(UWorld* World) { return World->SpawnActor(OriginalActor); }

	IPoolableActor* GetInterface(AActor* Actor);

	void HideActor(AActor* Actor);
	void ShowActor(AActor* Actor);


private:
	UPROPERTY()
	TSubclassOf<AActor> OriginalActor;

	UPROPERTY()
	TArray<AActor*> ObjectPool;
};
