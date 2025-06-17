// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientLocalStorageBase.h"
#include "Kismet/GameplayStatics.h"

UClientLocalStorageBase* UClientLocalStorageBase::LoadClientLocalStorage(FString StorageType, int32 UserIndex)
{
	if (StorageType.IsEmpty())
		return nullptr;

	USaveGame* StorageData = UGameplayStatics::LoadGameFromSlot(StorageType, UserIndex);
	return Cast<UClientLocalStorageBase>(StorageData);
}

bool UClientLocalStorageBase::SaveData()
{
	bool Result = UGameplayStatics::SaveGameToSlot(this, StorageType, UserIndex);

	if (Result)
	{
		UE_LOG(LogTemp, Log, TEXT("[ClientLocalStorageBase] Success Save ClientLocalStorage %s"), *StorageType);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[ClientLocalStorageBase] Fail Save ClientLocalStorage %s"), *StorageType);
	}

	return Result;
}

void UClientLocalStorageBase::CompleteSaveData()
{
}

void UClientLocalStorageBase::OnLoadData()
{
}

void UClientLocalStorageBase::CompleteLoadData(UWorld* World)
{
}
