// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/Public/UI/MVVM/Modular_MVVMlBase.h"
#include "AbilitySystemComponent.h"
#include "HDMVVM_MonsterInfo.generated.h"

struct FOnAttributeChangeData;

UCLASS()
class PROJECTH_API UHDMVVM_MonsterInfo : public UModular_MVVMlBase
{
	GENERATED_BODY()
public:
	virtual void Init(UAbilitySystemComponent* InASC) override;
public:
	void HandleDataChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(BlueprintReadWrite, FieldNotify)
	float CurrentHP = 0.0f;
	UPROPERTY(BlueprintReadWrite, FieldNotify)
	float OriginHP = 0.0f;
};
