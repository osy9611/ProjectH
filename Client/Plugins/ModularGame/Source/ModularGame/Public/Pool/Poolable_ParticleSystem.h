// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Poolable_ParticleSystem.generated.h"

class UParticleSystemComponent;
class UParticleSystem;

UCLASS()
class MODULARGAME_API UPoolable_ParticleSystem : public UObject
{
	GENERATED_BODY()
public:
	void SpawnRootActor(UWorld& InWorld);


private:
	UPROPERTY()
	TObjectPtr<AActor> SpawnOwnerActor;

	UPROPERTY()
	TArray<UParticleSystem*> ObjectPool;
};
