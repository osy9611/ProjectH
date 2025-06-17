// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Poolable_NiagaraSystem.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class MODULARGAME_API UPoolable_NiagaraSystem : public UObject
{
	GENERATED_BODY()
public:
	void SpawnRootActor(UWorld& InWorld);

	bool IsEmpty() { return ObjectPool.IsEmpty(); }
	int32 GetCount() { return ObjectPool.Num(); }

	void Init( int32 Count = 5);

	UNiagaraComponent* Get(UNiagaraSystem* NiagaraSystem,AActor* OwnerActor, bool IsActive);

	UFUNCTION(BlueprintCallable)
	void Return(UNiagaraComponent* NiagaraComp);

	UNiagaraComponent* Create();

private:
	UPROPERTY()
	TObjectPtr<AActor> SpawnOwnerActor;

	UPROPERTY()
	TArray<UNiagaraComponent*> ObjectPool;
};
