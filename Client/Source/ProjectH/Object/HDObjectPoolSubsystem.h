// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/Pool/ObjectPoolSubsystem.h"
#include "HDObjectPoolSubsystem.generated.h"

class UParticleSystemComponent;

UCLASS() 
class PROJECTH_API UHDObjectPoolSubsystem : public UObjectPoolSubsystem
{
	GENERATED_BODY()
public:
	void CreateParticle();

	UFUNCTION()
	void ReturnParticle(UParticleSystemComponent* Particle);
};
