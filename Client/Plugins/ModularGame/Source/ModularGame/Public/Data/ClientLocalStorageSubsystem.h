// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ClientLocalStorageSubsystem.generated.h"

class UClientLocalStorageBase;

UCLASS()
class MODULARGAME_API UClientLocalStorageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UClientLocalStorageSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void RegisterStorages(TArray<FString> InStorageTypes);

	void LoadAllStorage();
	void SaveAllStorage();

	UFUNCTION(BlueprintCallable)
	UClientLocalStorageBase* CreateStorage(const FString& StorageType, TSubclassOf<UClientLocalStorageBase> Storage);

	template<typename T = UClientLocalStorageBase>
	T* CreateStorageType(const FString& StorageType, TSubclassOf<UClientLocalStorageBase> Storage);

	UFUNCTION(BlueprintCallable)
	UClientLocalStorageBase* GetStorage(const FString& StorageType);

	template<typename T = UClientLocalStorageBase>
	T* GetStorageType(const FString& StorageType);

	UFUNCTION(BlueprintCallable)
	void SaveStorage(const FString& StorageType, UClientLocalStorageBase* Storage);

	void SaveStorage(const FString& StorageType);

	UFUNCTION(BlueprintCallable)
	bool CheckHasStorageData(const FString& Type) { return StorageDatas.Contains(Type); }

	UPROPERTY()
	int32 UserIndex = 0;

private:
	UPROPERTY()
	TArray<FString> StorageTypes;

	UPROPERTY()
	TMap<FString, UClientLocalStorageBase*> StorageDatas;
};

template<typename T>
T* UClientLocalStorageSubsystem::CreateStorageType(const FString& StorageType, TSubclassOf<UClientLocalStorageBase> Storage)
{
	UClientLocalStorageBase* NewStorage = CreateStorage(StorageType, Storage);
	if (NewStorage)
	{
		return Cast<T>(NewStorage);
	}
	return nullptr;
}

template<typename T>
T* UClientLocalStorageSubsystem::GetStorageType(const FString& StorageType)
{
	UClientLocalStorageBase* Storage = GetStorage(StorageType);
	if (Storage)
	{
		return Cast<T>(Storage);
	}

	return nullptr;
}
