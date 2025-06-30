// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Poolable.generated.h"

UINTERFACE(MinimalAPI)
class UPoolable : public UInterface
{
	GENERATED_BODY()
};

class MODULARGAME_API IPoolable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnActive();

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnDeActive();

	virtual void OnPoolGet() = 0;
	virtual void OnPoolReturn() = 0;
};
