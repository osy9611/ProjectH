// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "HDAttributeSet_Monster.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UHDAttributeSet_Monster : public UHDAttributeSet
{
	GENERATED_BODY()
public:
	UHDAttributeSet_Monster(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void OnInit(FString InitTableID) override;
	virtual void OnUpdateStatus() override;
};
