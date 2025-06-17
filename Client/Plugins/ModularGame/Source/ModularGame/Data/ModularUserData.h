// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ModularUserData.generated.h"

/**
 * 
 */
UCLASS()
class MODULARGAME_API UModularUserData : public UObject
{
	GENERATED_BODY()
public:
	virtual void RegisterData(TFunction<void(FString, bool)>UpdateContentCallBack);
	virtual void RegisterListener();
protected:
	TFunction<void(FString, bool)> Callback;
};
