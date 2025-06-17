// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectPoolSubsystem.generated.h"

class UPoolable_Actor;
class UPoolable_NiagaraSystem;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class MODULARGAME_API UObjectPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UObjectPoolSubsystem();

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION(BlueprintCallable)
	AActor* GetActor(TSubclassOf<AActor> ClassType);
	UFUNCTION(BlueprintCallable)
	void ReturnActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	UNiagaraComponent* GetNiagaraSystem(UNiagaraSystem* NiagaraSystem, AActor* OwnerActor, bool IsActive);
	UFUNCTION(BlueprintCallable)
	void ReturnNiagaraSystem(UNiagaraComponent* NiagaraComp);

private:
	UPROPERTY()
	TMap<UClass*, UPoolable_Actor*> ObjectPools;

	UPROPERTY()
	TObjectPtr<UPoolable_NiagaraSystem> NiagaraPools;
};
