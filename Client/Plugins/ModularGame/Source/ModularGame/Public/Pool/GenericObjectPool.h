// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GenericObjectPool.generated.h"

/**
 * 
 */
UCLASS()
class MODULARGAME_API UGenericObjectPool : public UObject
{
	GENERATED_BODY()
public:
	void Init(TFunction<UObject* (UWorld*)> InGenrator, int32 Count = 5, TFunction<void(UObject*,bool)>InInitializer = nullptr, TFunction<void(UObject*)>InDeInitializer = nullptr);

	UObject* Get(bool IsActive);
	void Return(UObject* ReturnObject);
	
private:
	TFunction<UObject* (UWorld*)> ObjectGenrator;
	TFunction<void(UObject*,bool)>Initializer;
	TFunction<void(UObject*)>DeInitializer;

	UPROPERTY()
	TArray<UObject*> ActiveObjects;

	UPROPERTY()
	TArray<UObject*> ObjectPool;
};
