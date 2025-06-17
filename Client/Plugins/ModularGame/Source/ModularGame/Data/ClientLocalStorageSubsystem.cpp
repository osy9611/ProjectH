// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientLocalStorageSubsystem.h"
#include "ClientLocalStorageBase.h"

UClientLocalStorageSubsystem::UClientLocalStorageSubsystem()
{
}

void UClientLocalStorageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UClientLocalStorageSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (!StorageDatas.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("[ClientLocalStorageSubsystem] SaveStorage"));
		SaveAllStorage();
	}
}

void UClientLocalStorageSubsystem::RegisterStorages(TArray<FString> InStorageTypes)
{
	StorageTypes = InStorageTypes;
}

void UClientLocalStorageSubsystem::LoadAllStorage()
{
	if (StorageTypes.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[ClientLocalStorageSubsystem] StorageTypes is Empty"));
		return;
	}


	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Log, TEXT("[ClientLocalStorageSubsystem] World is nullptr"));
		return;
	}

	for (int32 i = 0; i < StorageTypes.Num(); ++i)
	{
		UClientLocalStorageBase* StorageBase = UClientLocalStorageBase::LoadClientLocalStorage(StorageTypes[i], UserIndex);
		if (StorageBase)
		{
			StorageBase->StorageType = StorageTypes[i];
			StorageDatas.Add(StorageTypes[i], StorageBase);
			StorageBase->CompleteLoadData(GetWorld());
		}
	}
}

void UClientLocalStorageSubsystem::SaveAllStorage()
{
	for (auto& StorageData : StorageDatas)
	{
		StorageData.Value->SaveData();
	}
}

UClientLocalStorageBase* UClientLocalStorageSubsystem::CreateStorage(const FString& StorageType, TSubclassOf<UClientLocalStorageBase> Storage)
{
	if (StorageType.IsEmpty())
	{
		UE_LOG(LogTemp,Error,TEXT("[ClientLocalStorageBase] Create Storage Create Storage Faile This Storage SubclassOf Is nullptr"));
		return nullptr;
	}

	UClientLocalStorageBase* StorageData = nullptr;
	if (!StorageDatas.IsEmpty())
	{
		if (UClientLocalStorageBase** FindData = StorageDatas.Find(StorageType))
		{
			return *FindData;
		}
	}

	StorageData = NewObject<UClientLocalStorageBase>(this, Storage);
	StorageData->StorageType = StorageType;

	if (StorageData)
	{
		StorageDatas.Add(StorageType, StorageData);
	}

	return nullptr;
}

UClientLocalStorageBase* UClientLocalStorageSubsystem::GetStorage(const FString& StorageType)
{
	UClientLocalStorageBase* StorageData = *StorageDatas.Find(StorageType);
	return StorageData;
}

void UClientLocalStorageSubsystem::SaveStorage(const FString& StorageType, UClientLocalStorageBase* Storage)
{
	UClientLocalStorageBase* StorageData = nullptr;
	if (!StorageDatas.IsEmpty())
	{
		StorageData = *StorageDatas.Find(StorageType);
	}

	if (StorageData)
	{
		StorageData->SaveData();
	}
	else
	{
		StorageDatas.Add(StorageType, Storage);
		Storage->SaveData();
	}
}

void UClientLocalStorageSubsystem::SaveStorage(const FString& StorageType)
{
	if (StorageDatas.IsEmpty())
	{
		return;
	}

	UClientLocalStorageBase* StorageData = *StorageDatas.Find(StorageType);
	if (StorageData)
	{
		StorageData->SaveData();
	}
}



