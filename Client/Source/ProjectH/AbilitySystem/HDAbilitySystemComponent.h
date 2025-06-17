// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ModularGame/AbilitySystem/ModularAbilitySystemComponent.h"
#include "HDAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UHDAbilitySystemComponent : public UModularAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UHDAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
