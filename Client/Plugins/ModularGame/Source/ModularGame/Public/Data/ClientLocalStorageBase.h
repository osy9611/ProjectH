// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ClientLocalStorageBase.generated.h"

UCLASS()
class MODULARGAME_API UClientLocalStorageBase : public USaveGame
{
	GENERATED_BODY()

public:

	static UClientLocalStorageBase* LoadClientLocalStorage(FString StorageType, int32 UserIndex);

	virtual bool SaveData();
	virtual void CompleteSaveData();

	virtual void OnLoadData();
	virtual void CompleteLoadData(UWorld* World);

	FString StorageType;
	int32 UserIndex;	
};
